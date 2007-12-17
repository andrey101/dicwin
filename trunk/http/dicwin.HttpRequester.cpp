#include <QHttpResponseHeader>
#include <QCoreApplication>

#include "dicwin.TextEdit.h"
#include "dicwin.HttpRequester.h"

using namespace dicwin;

void HttpRequester::setURL(const QString& host, const QString& rest)
{
    a_host = host;
    a_rest = rest;
    setHost(a_host);
}

void HttpRequester::doGet()
{
    connect( this, SIGNAL(readyRead(const QHttpResponseHeader &)), this, SLOT(getResponse()));

    connect( this, SIGNAL(done(bool)), m_textedit, SLOT(print()));

    QString str("GET ");
    str += a_rest;
    str += " HTTP/1.1\r\nAccept: text/*\r\nUser-Agent: DicWin\r\nHost: ";
    str += a_host;
    str += "\r\n\r\n";
    //    cout << qPrintable(str) << endl;
    QHttpRequestHeader header(str);
    request(header);
}

void HttpRequester::getResponse()
{
    //cout << "Bytes de la peticion: " << bytesAvailable() << endl;
    QByteArray dataRead = readAll();
    a_result += QString::fromUtf8(dataRead);
}

void HttpRequester::finish()
{
    QCoreApplication::exit();
}

