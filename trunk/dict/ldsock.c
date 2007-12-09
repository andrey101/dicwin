/* ldsock.c
 *
 * This is where all the nitty-gritty socket code goes. 
 *
 *   Copyright (c) 2001, 2002 Steven Young
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "Winsock2.h"

#include "libdict.h"
#include "ldsock.h"
#include "ldutil.h"

int initWinSock() {

	WORD wVersionRequested;
	WSADATA wsaData;
	int err; 
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		printf("WSAStartup failed with err=%d\n", err);
	}
	return err;
}

/* open a TCP connection to ld_host on ld_port from the conn structure.
 * return true on success, false/dicterrno on fail. */
ld_bool ld_tcpopen(struct ld_conn *conn) {

	struct hostent *hptr;
	struct protoent *pptr;
	struct sockaddr_in saddr;
	int sockfd, proto, selret; //,sockparms;
	struct timeval timeout;
	fd_set writefds;
	char tmpbuf;
	int iMode = 1;
	int wsError = 0, recvRet = 0;

	conn->ld_errno = LDOK;

	/* resolve the host */
	if(!(hptr = gethostbyname(conn->ld_host))) {
		conn->ld_errno = LDERRNO;
		wsError = WSAGetLastError();
		if ( wsError != 0) {
			printf("error#:%d\n", wsError);
		}

		return(LD_False);
	}

	/* fill out ld_host with the 'real' hostname.  This is not necessarily
	 * required but may be useful in the future. */
	ld_xfree(conn->ld_host);
	conn->ld_host = ld_xstrdup(hptr->h_name);

	/* find the protocol */
	if(!(pptr = getprotobyname("tcp"))) {
		proto = 6;
	}
	else {
		proto = pptr->p_proto;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, proto)) == -1) {
		conn->ld_errno = LDERRNO;
		return(LD_False);
	}

	memcpy(&saddr.sin_addr, hptr->h_addr, hptr->h_length);
	saddr.sin_port = htons(conn->ld_port);
	saddr.sin_family = AF_INET;

	/* Temporarily set the socket blocking .. */
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.

	ioctlsocket(sockfd, FIONBIO, (u_long FAR*) &iMode);
	//sockparms = fcntl(sockfd, F_GETFL);
	//fcntl(sockfd, F_SETFL, sockparms | O_NONBLOCK);

	if(connect(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) != 0) {
		wsError = WSAGetLastError();
		//		if(errno != WSAEINPROGRESS) {
		if(wsError != WSAEINPROGRESS && wsError != WSAEWOULDBLOCK) {
			conn->ld_errno = LDERRNO;
			return(LD_False);
		}
	}

	if(conn->ld_timeout) {
		timeout.tv_sec = conn->ld_timeout;
		timeout.tv_usec = 0;
	}

	FD_ZERO(&writefds);
	FD_SET(sockfd, &writefds);

	selret = select(sockfd+1, &writefds, NULL, NULL, conn->ld_timeout ? &timeout : NULL);

	if(selret == 0) {
		conn->ld_errno = LDTIMEOUT;
		return(LD_False);
	}
	else if(selret < 0) {
		conn->ld_errno = LDERRNO;
		return(LD_False);
	}

	/* Reset the socket to blocking. */
	iMode = 0;
	ioctlsocket(sockfd, FIONBIO, (u_long FAR*) &iMode);
	//	fcntl(sockfd, F_SETFL, sockparms);

	//	if(_read(sockfd, &tmpbuf, 0) == -1) {
	recvRet = recv(sockfd, &tmpbuf, 0, 0);
	wsError = WSAGetLastError();

	if(recvRet == -1) {
		conn->ld_errno = LDERRNO;
		return(LD_False);
	}

	conn->ld_srvfd = sockfd;
	conn->ld_srvreply = NULL;

	return(LD_True);
}

/* Print a string to a socket; just like printf */
ld_bool ld_sockprintf(struct ld_conn *conn, char *fmt, ...) {

	va_list args;
	/* this limitation shouldn't be a problem */
	char outbuf[BUFSIZ];
	int wrcount=0, offset = 0;
	struct timeval timeout;
	fd_set writefds;
	time_t starttime = time(NULL);

	conn->ld_errno = LDOK;

	va_start(args, fmt);
	vsnprintf(outbuf, BUFSIZ, fmt, args);
	va_end(args);

	while(offset < strlen(outbuf)) {
		/* First, check if we are using a timeout, and if so, has it 
		 * expired? */
		if(conn->ld_timeout) {
			timeout.tv_sec = (starttime + conn->ld_timeout) - time(NULL);
			timeout.tv_usec = 0;
		}
		/* We don't bother checking for an error on this select because
		 * we will catch them on the write below anyway without duplicating
		 * code */
		FD_ZERO(&writefds);
		FD_SET(conn->ld_srvfd, &writefds);
		if((select(conn->ld_srvfd+1, NULL, &writefds, NULL, conn->ld_timeout ? &timeout : NULL)) == 0) {
			/* timeout expired */
			conn->ld_errno = LDTIMEOUT;
			return(LD_False);
		}

		//int send(SOCKET s,const char* buf,int len,int flags);
		wrcount = send(conn->ld_srvfd, outbuf + offset, strlen(outbuf) - offset, 0);
		//wrcount = write(conn->ld_srvfd, outbuf + offset, strlen(outbuf) - offset);
		if(wrcount < 0) {
			conn->ld_errno = LDERRNO;
			return(LD_False);
		}
		offset += wrcount;
		/* Check again for timeout */
		if(conn->ld_timeout) {
			if(time(NULL) > starttime + conn->ld_timeout) {
				conn->ld_errno = LDTIMEOUT;
				return(LD_False);
			}
		}
	}

	if(conn->ld_debug) 
		fprintf(stderr, "> %s", outbuf);
	return(LD_True);
}

/* read in a line of text from fd to sptr.  sptr is a pointer to where the
 * pointer to the input line is.  we also don't care how many bytes were
 * read, only that it worked or not.  this may allocate a byte or two
 * more than it needs in *sptr. */
ld_bool ld_readline(struct ld_conn *conn, char **sptr) {

	int slen = 0, soffset = 0;
	ld_bool readok = LD_False;
	struct timeval timeout;
	fd_set readfds;
	time_t starttime = time(NULL);
	unsigned int allocs = 1;
	int wsError = 0, recvRet = 0;

	conn->ld_errno = LDERRNO;

	/* we always deal with slen + 1 to accomodate the trailing NULL */
	*sptr = ld_xmalloc(LD_READBLOCK + 1);

	/* read a byte into the newest char of the buffer, unless socket
	 * goes bad, or ... */
	while(1) {
		/* Have we timed out? */
		if(conn->ld_timeout) {
			if(time(NULL) > (starttime + conn->ld_timeout)) 
				break;

			timeout.tv_sec = starttime + conn->ld_timeout - time(NULL);
			timeout.tv_usec = 0;
		}
		FD_ZERO(&readfds);
		FD_SET(conn->ld_srvfd, &readfds);
		if(select(conn->ld_srvfd+1, &readfds, NULL, NULL, conn->ld_timeout ? &timeout : NULL) == 0) {
			/* timed out */
			conn->ld_errno = LDTIMEOUT;
			break;
		}
		/* Data available, read a single byte.  This is lame. 
		 * Data should be dumped into a pool as fast as it can be read
		 * (i.e. in 8k blocks or somesuch) and scanned over instead. */
		recvRet = recv(conn->ld_srvfd, *sptr + slen, 1, 0);
		wsError = WSAGetLastError();
		if (recvRet < 1)
			break;

		//if(_read(conn->ld_srvfd, *sptr + slen, 1) < 1) 
		//	break;

		/* ... we see a \r\n or a \n. */
		if(*(*sptr + slen) == '\n') {
			if(slen) {
				if(*(*sptr + slen - 1) == '\r') {
					soffset = -1;
				}
			}
			*(*sptr + slen + soffset) = '\0';
			readok = LD_True;
			break;
		}
		slen++;
		if((LD_READBLOCK * allocs) - slen == 1) 
			*sptr = ld_xrealloc(*sptr, LD_READBLOCK * (allocs++) + 1);
	}

	if(conn->ld_timeout) {
		if(time(NULL) > starttime + conn->ld_timeout) {
			conn->ld_errno = LDTIMEOUT;
			readok = LD_False;
		}
	}

	if(!readok) {
		ld_xfree(*sptr);
		*sptr = NULL;
	} else {
		conn->ld_errno = LDOK;
	}

	if(conn->ld_debug) {
		if(readok) 
			fprintf(stderr, "< %s\n", *sptr);
		else
			fprintf(stderr, "< (libdict: read error)\n");
	}

	return(readok ? LD_True : LD_False);
}

/* this is just a wrapper for ld_readline that calls ld_xfree on the previous
 * string first */
ld_bool ld_xreadline(struct ld_conn *conn, char **sptr) {

	if(*sptr) {
		ld_xfree(*sptr);
		*sptr = NULL;
	}

	return(ld_readline(conn, sptr));
}
