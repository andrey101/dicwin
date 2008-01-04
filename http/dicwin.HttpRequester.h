#ifndef DICWIN_HTTPREQUESTER_H_
#define DICWIN_HTTPREQUESTER_H_

#include <QHttp>
#include <QString>

namespace dicwin {

class TextEdit;

class HttpRequester : public QHttp
{
    Q_OBJECT

public:
	HttpRequester(TextEdit* textedit) : QHttp(), m_textedit(textedit)
		{ qDebug("HttpRequester::HttpRequester()"); }
    void setURL(const QString& host, const QString& rest);
    void doGet();
    QString getResult() { return m_result; }

public slots:
   void getResponse();

private:
   QString m_host;
   QString m_rest;
   QString m_result;
   TextEdit* m_textedit;
};
}
#endif /*DICWIN_HTTPREQUESTER_H_*/
