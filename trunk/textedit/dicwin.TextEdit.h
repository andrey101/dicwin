#ifndef TEXTEDIT_H_
#define TEXTEDIT_H_

#include <QTextEdit>
#include "dicwin.DictWrapper.h"
#include "dicwin.HttpRequester.h"

class QClipboard;

namespace dicwin {

class DictWrapper;

class TextEdit : public QTextEdit
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);

public slots:
    void newClipboardText();
    void print();

private:
    QClipboard* m_clipboard;
    DictWrapper* m_dictWrapper;
    HttpRequester m_httpRequester;
    void getWord(QString& qword, QString& result);
};
}
#endif
