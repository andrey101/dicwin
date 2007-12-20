#include <QHttpResponseHeader>
#include <QCoreApplication>

#include "dicwin.TextEdit.h"
#include "dicwin.HttpRequester.h"

using namespace dicwin;

void HttpRequester::setURL(const QString& host, const QString& rest)
{
    qDebug("HttpRequester::setURL() | %s", qPrintable(host + rest));	
    m_host = host;
    m_rest = rest;
    setHost(m_host);
}

void HttpRequester::doGet()
{
    qDebug("HttpRequester::doGet()");	
	m_result.clear();
    connect( this, SIGNAL(readyRead(const QHttpResponseHeader &)), this, SLOT(getResponse()));
    connect( this, SIGNAL(done(bool)), m_textedit, SLOT(print()));

    QString str("GET ");
    str += m_rest;
    str += " HTTP/1.1\r\nAccept: text/*\r\nUser-Agent: DicWin\r\nHost: ";
    str += m_host;
    str += "\r\n\r\n";

    QHttpRequestHeader header(str);
    request(header);
}

void HttpRequester::getResponse()
{
    qDebug("HttpRequester::getResponse()");	
    //cout << "Bytes de la peticion: " << bytesAvailable() << endl;
    QByteArray dataRead = readAll();
    m_result += QString::fromUtf8(dataRead);
    m_textedit->show();
}


