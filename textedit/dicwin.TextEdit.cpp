#include <QApplication>
#include <QClipboard>
#include <QtDebug>
#include <QAbstractTextDocumentLayout>

#include "dicwin.TextEdit.h"
#include "dicwin.DictWrapper.h"

using namespace dicwin;

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent), m_clipboard(0), m_dictWrapper(0), m_httpRequester(this)
{
    qDebug("TextEdit::TextEdit() : (%s)", qPrintable(objectName()));
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(dataChanged()), this, SLOT(newClipboardText()));
}

void TextEdit::newClipboardText()
{
    qDebug("TextEdit::newClipboardText() : (%s)", qPrintable(objectName()));

    QString clipText("Clipboard:");
    QString clipContent = m_clipboard->text();
    QString line = "\n";
    QString dictResult;

    if (true) {
        QString host = "buscon.rae.es";
        QString rest = "/draeI/SrvltGUIBusUsual?TIPO_HTML=2&TIPO_BUS=3&LEMA=";
        rest += clipContent;
        m_httpRequester.setURL(host, rest);
        m_httpRequester.doGet();
    }
    else {

        m_dictWrapper = new dicwin::DictWrapper();
        m_dictWrapper->getWord(clipContent, dictResult);

        setHtml(clipText+clipContent+line+dictResult);
        QSize s = document()->documentLayout()->documentSize().toSize();
        resize(s);
        show();
    }
}

void TextEdit::print()
{
    QString text = m_httpRequester.getResult();
    clear();
    setHtml(text);
    QSize s = document()->documentLayout()->documentSize().toSize();
    resize(s);
}
