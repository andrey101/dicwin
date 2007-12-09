// libdict.hpp
// sdyoung@well.com
//
// This is the header file for the dictConn C++ wrapper.
// 
//   Copyright (c) 2001, 2002 Steven Young
//
//   Permission is hereby granted, free of charge, to any person obtaining
//   a copy of this software and associated documentation files (the
//   "Software"), to deal in the Software without restriction, including
//   without limitation the rights to use, copy, modify, merge, publish,
//   distribute, sublicense, and/or sell copies of the Software, and to
//   permit persons to whom the Software is furnished to do so, subject to
//   the following conditions:
//
//   The above copyright notice and this permission notice shall be
//   included in all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// $History$
// $Log: libdict-wrap.hpp,v $
// Revision 1.3  2002/03/15 18:43:07  sdyoung
// Added license info to comment block.
//
// Revision 1.2  2002/03/13 21:07:36  sdyoung
// Added strError and getError functions to deal with ld_errno.
//
// Revision 1.1  2002/03/12 18:42:42  sdyoung
// Initial revision
//

#ifndef DICWIN_DICTWRAPPER_H
#define DICWIN_DICTWRAPPER_H

extern "C" {
#include "libdict.h"
}

#include <QString>

namespace dicwin {

class DictWrapper {
 public:
    DictWrapper() : ldconn(0) { ; }
    ~DictWrapper() { if(ldconn) ld_freeconn(ldconn); }
    void getWord(QString& qstring, QString& result);

 private:
    char* getServerInfo() { return(ld_serverinfo(ldconn)); }
    char* getSrvReply() { return(ld_getsrvreply(ldconn)); }
    int getRespNo(char* msg) { return(ld_getrespno(msg)); }
    bool checkOk(int respno) { return(convToBool(ld_checkok(respno))); }

    bool newConn(char* host, int port = 0, int timeout = 0, char* client = 0, bool debug = false);
    bool closeConn() { return(convToBool(ld_closeconn(ldconn))); }

    bool auth(char* user, char* pw) { return(convToBool(ld_auth(ldconn, user, pw))); }
    struct ld_dbs** getDbs() { return(ld_getdbs(ldconn)); }
    struct ld_dbs** getStrats() { return(ld_getstrats(ldconn)); }
    bool setDb(char* db) { return(convToBool(ld_setdb(ldconn, db))); }
    bool setStrat(char* strat) { return(convToBool(ld_setstrat(ldconn, strat))); }

    struct ld_defanswer** define(char *word) { return(ld_define(ldconn, word)); }
    struct ld_matchanswer** match(char *word) { return(ld_match(ldconn, word)); }

    int getError() { return(ld_geterrno(ldconn)); }
    char* strError(int errnum) { return(ld_strerror(errnum)); }

    bool convToBool(ld_bool val)
    { 
	if(val == LD_True) return true;
	return false;
    }

    ld_bool convToLDBool(bool val)
    {
	if(val == true) return LD_True;
	return LD_False;
    }

 private:
    struct ld_conn* ldconn;
};
}

#endif
