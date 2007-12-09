/* ldutil.h
 *
 * This is the header file for ldutil.c - see that file for details
 * on what this is all about.  This is just prototypes.
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
#ifndef _LDUTIL_H
#define _LDUTIL_H
#include <stdlib.h>
#include "libdict.h"

#ifndef MDEBUG
void *ld_xmalloc(size_t sz);
char *ld_xstrdup(char *s);
void ld_xfree(void *p);
void *ld_xrealloc(void *p, size_t sz);
#else
#define ld_xmalloc malloc
#define ld_xstrdup strdup
#define ld_xfree free
#define ld_xrealloc realloc
#endif /* !MDEBUG */

char *ld_unquote(char *str);

#endif /* !_LDUTIL_H */
