/* libdict.c
 *
 * This is where the higher-level functions are handled.  Low-level
 * (and mostly internal) functions are in ldsock.c and ldutil.c.
 *
 * The guideline is that this file is where all exported functions
 * go.  This guideline is gleefully violated with considerable frequency.
 *
 *   Copyright (c) 2001, 2002 Steven Young
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <stdio.h>
//#include <netdb.h>
#include <string.h>
//#include <errno.h>
#include "Winsock2.h"
//#include <unistd.h>
#include <ctype.h>
#include <stdarg.h>

#include "libdict.h"
#include "ldutil.h"
#include "ldsock.h"

/* get server response number.  returns 0 on error with ld_errno set. */
/* note: now uses the ld_servcode enumerated type.  see ldservcodes.h */
enum ld_servcode ld_getrespno(char *msg) {
	char num[4];

	if((strlen(msg) < 3) || ((!isdigit(msg[0])) || (!isdigit(msg[1])) || (!isdigit(msg[2])))) {
		return(LD_None);
	} 

	memcpy(num, msg, 3);
	num[3] = '\0';

	return((enum ld_servcode) atoi(num));
}

/* ld_checkok will take an integer response number and simplify it for
 * you: is it success (LD_True), failure (LD_False), or was it an intermittent
 * failure (Maybe). */
ld_bool ld_checkok(int respno) {
	if(respno < 300)
		return(LD_True);
	if(respno > 399)
		return(LD_False);
	return(LD_Maybe);
}

/* get a server reply; check it for okayness.  this is used by some
 * of the simpler commands that don't deal with variable-length
 * responses */
ld_bool ld_docommand(struct ld_conn *conn, ld_bool getr, char *fmt, ...) {
	ld_bool retv = LD_True;
	va_list args;
	/* sorry guys - 8k command limit */
	char buf[BUFSIZ];

	/* first, send the command */
	va_start(args, fmt);
	vsnprintf(buf, BUFSIZ, fmt, args);
	va_end(args);

	if((ld_sockprintf(conn, buf) == LD_False)) {
		retv = LD_False;
		/* getr tells us if we need to grab and do minimal processing
		 * on the return string ourselves.  this can save a lot of 
		 * time. */
	} else if(getr == LD_True && ld_xreadline(conn, &conn->ld_srvreply) == LD_False) {
		retv = LD_False;
	} else if(getr == LD_True) {
		retv = ld_checkok(ld_getrespno(conn->ld_srvreply));
	}

	return(retv);
}

/* send the CLIENT info to the server */
ld_bool ld_sendclient(struct ld_conn *conn, char *client) {
	if(client) 
		return(ld_docommand(conn, LD_True, "CLIENT %s\r\n", client));
	return(ld_docommand(conn, LD_True, "CLIENT %s\r\n", LD_VERSION));
}

/* get database list.  also used to retrieve strategies */
struct ld_dbs **ld_getdblist(struct ld_conn *conn, char *cmd) {
    char *tmpbuf = NULL;
    struct ld_dbs **retv = NULL;
    int dbcount = 0;
    char *p;
    ld_bool readok;

    /* execute the command (show db or show strat) */
    if(ld_docommand(conn, LD_True, "%s\r\n", cmd) == LD_False)
        return(NULL);

    while((readok = ld_xreadline(conn, &tmpbuf)) == LD_True) {
        /* while the data hasn't ended ... */
        if(!strcmp(tmpbuf, ".")) break;

        /* create an array to hold our answer's attributes.
         * or, resize it upwards for new entries */
        retv = ld_xrealloc(retv, sizeof(struct ld_dbs *) * (dbcount + 2));
        /* point our array slot to attribute structure */
        retv[dbcount] = ld_xmalloc(sizeof(struct ld_dbs));
        memset(retv[dbcount], 0, sizeof(struct ld_dbs));
        //bzero(retv[dbcount], sizeof(struct ld_dbs));

        if((p = strchr(tmpbuf, ' '))) {
            *p = '\0';
            retv[dbcount]->ld_dbname = ld_xstrdup(ld_unquote(tmpbuf));
            retv[dbcount]->ld_dbdesc = ld_xstrdup(ld_unquote(p+1));
        } else {
            retv[dbcount]->ld_dbname = ld_xstrdup(ld_unquote(tmpbuf));
        }
        dbcount++;
    }

    retv[dbcount] = NULL;

    if(!readok) {
        ld_freedbs(retv);
        return(NULL);
    } else {
        if(ld_xreadline(conn, &conn->ld_srvreply) == LD_False) {
            ld_freedbs(retv);
            return(NULL);
        }
    }

    if(tmpbuf) {
        ld_xfree(tmpbuf);
        tmpbuf = NULL;
    }

    return(retv);
}

/* get a list of databases (internal) */
struct ld_dbs **ld_igetdbs(struct ld_conn *conn) {
    return(ld_getdblist(conn, "SHOW DB"));
}

/* get a list of strategies (internal) */
struct ld_dbs **ld_igetstrats(struct ld_conn *conn) {
    return(ld_getdblist(conn, "SHOW STRAT"));
}

/* connect to a dict server.  everything except the host argument is optional
 * and can be set to 0 or NULL (whichever is appropriate).
 * this will spit you back a filled out ld_conn structure, or NULL upon
 * error. */
struct ld_conn *ld_newconn(char *host, int port, int timeout, char *client, ld_bool debug) {
	struct ld_conn *srvhandle;
	//struct servent *dictport;

	srvhandle = ld_xmalloc(sizeof(struct ld_conn));
	memset(srvhandle, 0, sizeof(struct ld_conn));
	//	bzero(srvhandle, sizeof(struct ld_conn));
	
	srvhandle->ld_host = ld_xstrdup(host);
	srvhandle->ld_timeout = timeout;
	srvhandle->ld_debug = debug;

	/* try and work out the port number for ourselves if it's not passed. */
	//if(!port) {
	//	if(!(dictport = getservbyname("dict", "tcp"))) {
			port = LD_DEFPORT;
	//	} else {
	//		port = dictport->s_port;
	//	}
	//}
	
	srvhandle->ld_port = port;

	if(ld_tcpopen(srvhandle) == LD_False) 
		return(srvhandle);

	/* ld_tcpopen has filled out ld_srvfd for us */
	
	/* now we just slurp the banner and we're done. */
	if(ld_readline(srvhandle, &srvhandle->ld_srvbanner) == LD_False) {
		srvhandle->ld_errno = LDERRNO;
		closesocket(srvhandle->ld_srvfd);
//		close(srvhandle->ld_srvfd);
		srvhandle->ld_srvfd = 0;
		return(srvhandle);
	}

	if((ld_checkok(ld_getrespno(srvhandle->ld_srvbanner)) != LD_True) ||
		(ld_sendclient(srvhandle, client) == LD_False)) {
		srvhandle->ld_errno = LDBADPROTO;
		return(srvhandle);
	}

	/* get databases */
	if((srvhandle->ld_dbs = ld_igetdbs(srvhandle))) 
		srvhandle->ld_strats = ld_igetstrats(srvhandle);

	return(srvhandle);
}

/* free the memory structures for conn */
void ld_freeconn(struct ld_conn *conn) {
	/* this does NOT set ld_errno because it is called after
	 * ld_errno has been set all over the place. */
	if(!conn) return;
	if(conn->ld_srvfd) 
		ld_closeconn(conn);
	if(conn->ld_host)
		ld_xfree(conn->ld_host);
	if(conn->ld_srvreply)
		ld_xfree(conn->ld_srvreply);
	if(conn->ld_srvbanner)
		ld_xfree(conn->ld_srvbanner);
	if(conn->ld_srchdb)
		ld_xfree(conn->ld_srchdb);
	if(conn->ld_srchstrat)
		ld_xfree(conn->ld_srchstrat);
	if(conn->ld_dbs) 
		if(*conn->ld_dbs) 
			ld_freedbs(conn->ld_dbs);
	if(conn->ld_strats) 
		if(*conn->ld_strats)
			ld_freedbs(conn->ld_strats);
	if(conn->ld_manswers)
		ld_freematans(conn->ld_manswers);
	if(conn->ld_danswers)
		ld_freedefans(conn->ld_danswers);
	ld_xfree(conn);
}

/* Perform the protocol-level teardown of connection conn.  We don't
 * check a lot of return values here since we don't really care. 
 * it will return true if the disconnect went smoothly and 
 * conn->ld_srvreply has the server's goodbye message in it.
 * false if things didn't go so well in which case conn->srvreply
 * is unspecified. */
ld_bool ld_closeconn(struct ld_conn *conn) {
	ld_bool retv = LD_False;
	/* tell the server we want to quit */
	if(conn && (ld_sockprintf(conn, "QUIT\r\n") == LD_True)) {
		/* the server connection is still open, so we
		 * politely wait for the server to say goodbye */
		ld_xreadline(conn, &conn->ld_srvreply); 
		retv = LD_True;
		if(conn->ld_srvfd) {
			closesocket(conn->ld_srvfd);
//			close(conn->ld_srvfd);
			conn->ld_srvfd = 0;
		}
	}
	return(retv);
}

/* authenticate user with the server */
ld_bool ld_auth(struct ld_conn *conn, char *user, char *pw) {
	return(ld_docommand(conn, LD_True, "AUTH %s %s\r\n", user, pw));
}

/* return server banner */
char *ld_serverinfo(struct ld_conn *conn) {
	return(conn->ld_srvbanner);
}

/* free a database list */
void ld_freedbs(struct ld_dbs **dbs) {
	int i = 0;

	if(!dbs) return;

	while(dbs[i]) {
		if(dbs[i]->ld_dbname) ld_xfree(dbs[i]->ld_dbname);
		if(dbs[i]->ld_dbdesc) ld_xfree(dbs[i]->ld_dbdesc);
		ld_xfree(dbs[i]);
		i++;
	}

	ld_xfree(dbs);
}
	
struct ld_dbs **ld_getdbs(struct ld_conn *conn) {
	return(conn->ld_dbs);
}

struct ld_dbs **ld_getstrats(struct ld_conn *conn) {
	return(conn->ld_strats);
}

/* set the strategy to use */
ld_bool ld_setstrat(struct ld_conn *conn, char *strat) {
	int x = 0;
	ld_bool stratok = LD_False;

	while(conn->ld_strats[x]) {
		if(!strcmpi(conn->ld_strats[x]->ld_dbname, strat)) {
//		if(!strcasecmp(conn->ld_strats[x]->ld_dbname, strat)) {
			stratok = LD_True;
			break;
		}
		x++;
	}

	if(stratok == LD_False) {
		conn->ld_errno = LDBADARG;
		return(LD_False);
	}

	if(conn->ld_srchstrat) ld_xfree(conn->ld_srchstrat);

	conn->ld_srchstrat = ld_xstrdup(strat);
	return(LD_True);
}

/* set the current db to be searched */
ld_bool ld_setdb(struct ld_conn *conn, char *db) {
	int x = 0;
	ld_bool dbok = LD_False;

	if(strcmp(db, "*")) {
		while(conn->ld_dbs[x]) {
			if(!strcmpi(conn->ld_dbs[x]->ld_dbname, db)) {
//			if(!strcasecmp(conn->ld_dbs[x]->ld_dbname, db)) {
				dbok = LD_True;
				break;
			}
			x++;
		}
	} else {
		dbok = LD_True;
	}

	if(dbok == LD_False) {
		conn->ld_errno = LDBADARG;
		return(LD_False);
	}

	if(conn->ld_srchdb) ld_xfree(conn->ld_srchdb);
	
	conn->ld_srchdb = ld_xstrdup(db);
	return(LD_True);
}

/* define a word.  we're not as paranoid as we should be here */
struct ld_defanswer **ld_define(struct ld_conn *conn, char *word) {
	char *tmpbuf = NULL, *p, *p2;
	int anscount = 0, anssz, cmdret;
	ld_bool readok;

	if(conn->ld_danswers) {
		ld_freedefans(conn->ld_danswers);
		conn->ld_danswers = NULL;
	}

	/* send the initial command */
	if(ld_docommand(conn, LD_False, "DEFINE %s %s\r\n", 
	                conn->ld_srchdb ? conn->ld_srchdb : "*",
					word) == LD_False) 
		return(NULL);
	
	/* get response.. */
	if(ld_xreadline(conn, &tmpbuf) == LD_False) 
		return(NULL);

	cmdret = ld_getrespno(tmpbuf);

	/* check there are some answers */
	if(cmdret != LD_DefsFollow) {
		conn->ld_errno = LDNOMATCH;
		ld_xfree(tmpbuf);
		tmpbuf = NULL;
		return(NULL);
	}

	/* read in all the defs */
	while((readok = ld_xreadline(conn, &tmpbuf)) == LD_True) {
		if(ld_getrespno(tmpbuf) == LD_OK)
			break;
		anssz = 0;
		conn->ld_danswers = ld_xrealloc(conn->ld_danswers, sizeof(struct ld_defanswer *) * (anscount + 2));
		conn->ld_danswers[anscount] = ld_xmalloc(sizeof(struct ld_defanswer));
		memset(conn->ld_danswers[anscount], 0, sizeof(struct ld_defanswer));
		//bzero(conn->ld_danswers[anscount], sizeof(struct ld_defanswer));

		if(ld_getrespno(tmpbuf) != LD_WordDBName) {
			readok = LD_False;
			conn->ld_errno = LDBADPROTO;
			break;
		}

		/* ok, let's deal with the 151 line */
		if(!(p=strchr(tmpbuf + 4, ' '))) {
			readok = LD_False;
			conn->ld_errno = LDBADPROTO;
			break;
		}

		if(!(p2=strchr(p + 1, ' '))) {
			readok = LD_False;
			conn->ld_errno = LDBADPROTO;
			break;
		}

		*p2 = '\0';

		conn->ld_danswers[anscount]->ld_ansdict = ld_xstrdup(ld_unquote(p + 1));

		*p = '\0';

		conn->ld_danswers[anscount]->ld_answord = ld_xstrdup(ld_unquote(tmpbuf + 4));

		while((readok = ld_xreadline(conn, &tmpbuf)) == LD_True) {
			if(!strcmp(tmpbuf, ".")) {
				break;
			}
			if(conn->ld_danswers[anscount]->ld_ansdef) {
				conn->ld_danswers[anscount]->ld_ansdef = ld_xrealloc(conn->ld_danswers[anscount]->ld_ansdef, strlen(conn->ld_danswers[anscount]->ld_ansdef) + strlen(tmpbuf) + 3);
				strcat(conn->ld_danswers[anscount]->ld_ansdef, "\r\n");
				strcat(conn->ld_danswers[anscount]->ld_ansdef, tmpbuf);
			} else {
				conn->ld_danswers[anscount]->ld_ansdef = ld_xstrdup(tmpbuf);
			}
		}

		if(readok == LD_False)
			break;

		anscount++;
	}

	if(readok == LD_False) {
		if(conn->ld_danswers[anscount]) {
			if(conn->ld_danswers[anscount]->ld_ansdict) ld_xfree(conn->ld_danswers[anscount]->ld_ansdict);
			if(conn->ld_danswers[anscount]->ld_answord) ld_xfree(conn->ld_danswers[anscount]->ld_answord);
			if(conn->ld_danswers[anscount]->ld_ansdef) ld_xfree(conn->ld_danswers[anscount]->ld_ansdef);
			ld_xfree(conn->ld_danswers[anscount]);
			conn->ld_danswers[anscount] = NULL;
		}
		ld_freedefans(conn->ld_danswers);
		conn->ld_danswers = NULL;
	}

	if(readok) {
		conn->ld_danswers[anscount] = NULL;
	}

	if(tmpbuf) {
		ld_xfree(tmpbuf);
		tmpbuf = NULL;
	}

	return(conn->ld_danswers);
}

/* free an answer */
void ld_freedefans(struct ld_defanswer **answers) {
	int x = 0;

	if(!answers) return;

	while(answers[x]) {
		if(answers[x]->ld_ansdict) ld_xfree(answers[x]->ld_ansdict);
		if(answers[x]->ld_answord) ld_xfree(answers[x]->ld_answord);
		if(answers[x]->ld_ansdef) ld_xfree(answers[x]->ld_ansdef);
		ld_xfree(answers[x]);
		x++;
	}

	ld_xfree(answers);
}

/* free the answer to a match */
void ld_freematans(struct ld_matchanswer **answers) {
	int x=0;

	if(!answers) return;

	while(answers[x]) {
		if(answers[x]->ld_ansdict) ld_xfree(answers[x]->ld_ansdict);
		if(answers[x]->ld_answord) ld_xfree(answers[x]->ld_answord);
		ld_xfree(answers[x]);
		x++;
	}

	ld_xfree(answers);
}

/* this kind of shames me .. watch out for some casting */
struct ld_matchanswer **ld_match(struct ld_conn *conn, char *word) {
	char tmpbuf[BUFSIZ];

	if(conn->ld_manswers) {
		ld_freematans(conn->ld_manswers);
		conn->ld_manswers = NULL;
	}
	_snprintf(tmpbuf, BUFSIZ, "MATCH %s %s %s", 
		     conn->ld_srchdb ? conn->ld_srchdb : "*",
		     conn->ld_srchstrat ? conn->ld_srchstrat : "exact",
		     word);
	return(conn->ld_manswers = (struct ld_matchanswer **)ld_getdblist(conn, tmpbuf));
}

/* this will return the server's response to the last command */
char *ld_getsrvreply(struct ld_conn *conn) {
	return(conn->ld_srvreply);
}

/* return error value from a connection */
int ld_geterrno(struct ld_conn *conn) {
	return(conn->ld_errno);
}
