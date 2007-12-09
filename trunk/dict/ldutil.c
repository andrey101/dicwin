/* ldutil.c
 *
 * Various utility functions that are used throughout libdict can
 * be found here. 
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
#include <stdlib.h>
#include <string.h>

#include "libdict.h"
#include "ldutil.h"

/* All this is pretty self-explanitory.  If MDEBUG is defined, we're 
 * using dmalloc, in which case all these functions are macros for
 * the "real" function (ld_xmalloc becomes malloc, etc) so dmalloc
 * can give meaningful results. */
#ifndef MDEBUG
void *ld_xmalloc(size_t sz) {
    void *p;

    if(!(p = malloc(sz))) {
	fprintf(stderr, "libdict panic: out of memory?");
	/* we will surely core now */
	return(NULL);
    }

    return(p);
}

char *ld_xstrdup(char *s) {
    char *p;

    if(!(p = strdup(s))) {
	fprintf(stderr, "libdict panic: unable to strdup %p\n", s);
	return(NULL);
    }

    return(p);
}

void ld_xfree(void *p) {
    if(p) free(p);
}

void *ld_xrealloc(void *p, size_t sz) {
    if(!(p = realloc(p, sz))) {
	fprintf(stderr, "libdict panic: unable to realloc\n");
	return(NULL);
    }
	
    return(p);
}
#endif /* !MDEBUG */

/* unquote a string */
char *ld_unquote(char *str) {
    static char buf[BUFSIZ];
    char *cpstart;
    int sz;

    if(str[0] == '\"') 
	cpstart = str + 1;
    else
	cpstart = str;
	
    if(str[strlen(str)-1] == '\"') 
	sz = (strlen(str) - 2);
    else
	sz = (strlen(str));

    memcpy(buf, cpstart,  sz);
    buf[sz] = '\0';

    return(buf);
}
