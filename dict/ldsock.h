/* ldsock.h
 *
 * This is where all the socket code is prototyped.  A few structures,
 * #defines and typedefs might be slipped in here too.
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
#ifndef _LDSOCK_H
#define _LDSOCK_H
#include "libdict.h"

/* first, defines. */
#define LD_READBLOCK 2048

/* second, protos */
ld_bool ld_tcpopen(struct ld_conn *conn);
ld_bool ld_sockprintf(struct ld_conn *, char *fmt, ...);
ld_bool ld_readline(struct ld_conn *, char **sptr);
ld_bool ld_xreadline(struct ld_conn *, char **sptr);

#endif /* !_LDSOCK_H */
