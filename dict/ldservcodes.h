/* ldservcodes.h
 *
 * This file declares the enumerated type ld_servresp, which represents
 * all the different response messages a dict server can send back.
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
#ifndef _LDSERVCODES_H
#define _LDSERVCODES_H

enum ld_servcode  { LD_None = 0, LD_DBCount = 110, LD_StratCount = 111,
		    LD_DBInfo = 112, LD_Help = 113, LD_ServInfo = 114,
		    LD_Challenge = 130, LD_DefsFollow = 150, LD_WordDBName = 151,
		    LD_MatchFollow = 152, LD_Stats = 210, LD_Caps = 220,
		    LD_Closing = 221, LD_AuthOK = 230, LD_OK = 250,
		    LD_SendResp = 330, LD_ServTempFail = 420, LD_ServShutdown = 421,
		    LD_Syntax = 500, LD_BadParams = 501, LD_CmdNotImpl = 502,
		    LD_ArgNotImpl = 503, LD_AccessDenied = 530, LD_AccessDeniedInfo = 531,
		    LD_AccessDeniedUnk = 532, LD_BadDB = 550, LD_BadStrat = 551,
		    LD_NoMatch = 552, LD_NoDBs = 554, LD_NoStrats = 555 };

#endif /* !_LDSERVCODES_H */
