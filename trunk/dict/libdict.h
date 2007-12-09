/* libdict.h
 *
 * This is the main header file for libdict.  All programs that want to use
 * libdict should include this file or things will go horribly amiss.
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
#ifndef _LIBDICT_H
#define _LIBDICT_H
#include "ldservcodes.h"

#ifdef MDEBUG
#include "dmalloc.h"
#endif

/* first, some basic #defines .. */
#define LD_VERSION "libdict 0.9"
#define LD_MAJVER 0
#define LD_MINVER 9

#define LD_DEFPORT 2628

/* second, some typedefs */
typedef enum { LD_False = 0, LD_True, LD_Maybe } ld_bool;

/* third, structures. */
/* this structure is the principle 'connection handle' that gets passed
 * to all libdict functions */
struct ld_conn {
	char *ld_host;
	int ld_port;
	int ld_timeout;
	int ld_srvfd;
	int ld_errno;
	ld_bool ld_debug;
	char *ld_srvreply;
	char *ld_srvbanner;
	char *ld_srchdb;
	char *ld_srchstrat;
	struct ld_dbs **ld_dbs;
	struct ld_dbs **ld_strats;
	struct ld_matchanswer **ld_manswers;
	struct ld_defanswer **ld_danswers;
};

/* this is a list, either of databases or strategies.  either way we
 * store it the same. */
struct ld_dbs {
	char *ld_dbname;
	char *ld_dbdesc;
};

/* this is where match results get stored */
struct ld_matchanswer {
	char *ld_ansdict;
	char *ld_answord;
};

/* this is a list of answers from the server for a particular define/
 * match. */
struct ld_defanswer {
	char *ld_answord;
	char *ld_ansdict;
	char *ld_ansdef;
};

/* fourth, prototypes */
enum ld_servcode ld_getrespno(char *msg);
ld_bool ld_checkok(int respno);
ld_bool ld_docommand(struct ld_conn *conn, ld_bool getr, char *fmt, ...);
struct ld_conn *ld_newconn(char *host, int port, int timeout, char *client, ld_bool debug);
void ld_freeconn(struct ld_conn *conn);
ld_bool ld_closeconn(struct ld_conn *conn);
ld_bool ld_auth(struct ld_conn *conn, char *user, char *pw);
char *ld_serverinfo(struct ld_conn *conn);
void ld_freedbs(struct ld_dbs **dbs);
struct ld_dbs **ld_getdbs(struct ld_conn *conn);
struct ld_dbs **ld_getstrats(struct ld_conn *conn);
ld_bool ld_setstrat(struct ld_conn *conn, char *strat);
ld_bool ld_setdb(struct ld_conn *conn, char *db);
struct ld_defanswer **ld_define(struct ld_conn *conn, char *word);
struct ld_matchanswer **ld_match(struct ld_conn *conn, char *word);
void ld_freedefans(struct ld_defanswer **answers);
void ld_freematans(struct ld_matchanswer **answers);
char *ld_getsrvreply(struct ld_conn *conn);
int ld_geterrno(struct ld_conn *conn);

/* finally, one last thing to do. */
#include "lderrno.h"

#endif /* !_LIBDICT_H */
