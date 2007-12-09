/* lderrno.h
 *
 * This is where the declarations for ld_errno go.  ld_errno
 * is almost always set to the numeric code of the server return
 * message, however it may also be set to LDERRNO which tells the
 * client to look in regular errno. 
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
#ifndef _LDERRNO_H
#define _LDERRNO_H

/* only two #defines here */
#define LDOK 0			/* nothing wrong */
#define LDERRNO 1		/* see errno */
#define LDBADARG 2		/* function was passed a bad argument */
#define LDNOMATCH 3		/* no matches for query */
#define LDBADPROTO 4	/* something strange happened with the protocol */
#define LDTIMEOUT 5		/* operation timed out */
#define LDERRMAX 5		/* maximum value for ld_errno */

/* protos */
char *ld_strerror(int errnum);

#endif /* !_LDERRNO_H */
