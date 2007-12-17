#ifndef HTTPREQUESTER_H_
#define HTTPREQUESTER_H_

#include <QHttp>
#include <QString>

namespace dicwin {

class TextEdit;

class HttpRequester : public QHttp
{
    Q_OBJECT

public:
    HttpRequester(TextEdit* textedit) : QHttp(), m_textedit(textedit) { ; }
    void setURL(const QString& host, const QString& rest);
    void doGet();
    QString getResult() { return a_result; }

public slots:
   void getResponse();
   void finish();

private:
   QString a_host;
   QString a_rest;
   QString a_result;
   TextEdit* m_textedit;
};
}
#endif /*HTTPREQUESTER_H_*/
