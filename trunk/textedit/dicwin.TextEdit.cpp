#include <QApplication>
#include <QClipboard>
#include <QtDebug>
#include <QAbstractTextDocumentLayout>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include "dicwin.TextEdit.h"
#include "dicwin.DictWrapper.h"

using namespace dicwin;

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent), m_clipboard(0), m_dictWrapper(0), m_httpRequester(this)
{
    qDebug("TextEdit::TextEdit()");
    m_clipboard = QApplication::clipboard();
    connect(m_clipboard, SIGNAL(dataChanged()), this, SLOT(newClipboardText()));
    
    createTrayActions();
    createTrayIcon();
    
    QIcon icon("./images/heart.svg");
    m_trayIcon->setIcon(icon);
    m_trayIcon->show();
}

void TextEdit::createTrayActions()
{
    qDebug("TextEdit::createTrayActions()");
	
    m_showAction = new QAction(tr("&Show"), this);
    connect(m_showAction, SIGNAL(triggered()), this, SLOT(show()));

    m_hideAction = new QAction(tr("&Hide"), this);
    connect(m_hideAction, SIGNAL(triggered()), this, SLOT(hide()));

    m_quitAction = new QAction(tr("&Quit"), this);
    connect(m_quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));	
}

void TextEdit::createTrayIcon()
{
	qDebug("TextEdit::createTrayIcon()");
	   
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_showAction);
    m_trayIconMenu->addAction(m_hideAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
}

void TextEdit::newClipboardText()
{
    qDebug("TextEdit::newClipboardText()");

    QString clipContent = m_clipboard->text();
    QString dictResult;

	this->clear(); // Clears the document content
	
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

        setHtml(dictResult);
        QSize s = document()->documentLayout()->documentSize().toSize();
        resize(s);
    }
}

void TextEdit::print()
{
    qDebug("TextEdit::print()");
    
    //QString httpResult = m_httpRequester.getResult();
    this->setHtml(m_httpRequester.getResult());
    this->show();
    
//    QTextDocument* textDocument = document();
//    QAbstractTextDocumentLayout* documentLayout = textDocument->documentLayout();
//    QSizeF sizeF = documentLayout->documentSize();
//    QSize s = sizeF.toSize();
    QSize s = document()->documentLayout()->documentSize().toSize();
    this->resize(s);
}
