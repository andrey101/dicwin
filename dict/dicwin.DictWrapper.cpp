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
//
#include <QtDebug>
#include "dicwin.DictWrapper.h"

extern "C" int initWinSock();

using namespace dicwin;

bool DictWrapper::newConn(char *host, int port, int timeout, char *client, bool debug)
{
    ldconn = ld_newconn(host, port, timeout, client, convToLDBool(debug));
    if(!ldconn)
	return(false);
    return(true);
}

void DictWrapper::getWord(QString& qword, QString& result)
{
    qDebug("DictWrapper::getWord() : (%s)", qPrintable(qword));

    QByteArray qbword(qword.toUtf8());
    char* word = qbword.data();
    
    char* server = "localhost";
    char* dict = NULL;
    char* strat = NULL;
    struct ld_defanswer** defs;
    int idx = 0;
    bool debug = false;

    int err = initWinSock();
    if (err != 0) {
	result = "WinSock Error\n";
    }
    else if (newConn(server, 0, 0, NULL, debug) == false) {
	int error = getError();
	result = "Unable to connect to server: ";
	result += strError(error);
    }
    else if (dict && !setDb(dict)) {
	result = "Invalid database specified.";
    }
    else if (strat && !setStrat(strat)) {
	result = "Invalid strategy specified.";
    }
    else if (!(defs = define(word))) {
	result = "Word not found.";
    }
    else {
	result = "Word found, definition follows.\n\n";
	idx = 0;
	while(defs[idx]) {
	    result += "Word: "; result += QString::fromUtf8(defs[idx]->ld_answord);
	    result += " ("; result += QString::fromUtf8(defs[idx]->ld_ansdict); result += ")";
	    result += "Definition: "; result += QString::fromUtf8(defs[idx]->ld_ansdef);
	    idx++;
	}
    }
    qDebug(qPrintable(result));
}
