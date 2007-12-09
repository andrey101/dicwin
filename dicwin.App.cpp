#include <QApplication>
#include <QClipboard>
#include <QtDebug>

#include "dicwin.App.h"
#include "dicwin.DictWrapper.h"

using namespace dicwin;

App::App(QWidget *parent) : QMessageBox(parent), m_clipboard(0), m_dictWrapper(0)
{
    qDebug("App::App() : (%s)", qPrintable(objectName()));
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(dataChanged()),
	    this, SLOT(showClipText()));
}

void App::showClipText()
{
    qDebug("App::showClipText() : (%s)", qPrintable(objectName()));

    QString clipText("Clipboard:");
    QString clipContent = m_clipboard->text();
    QString line = "\n";
    QString dictResult;

    m_dictWrapper = new dicwin::DictWrapper();
    m_dictWrapper->getWord(clipContent, dictResult);
    
    setText(clipText+clipContent+line+dictResult);
}
