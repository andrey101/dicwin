#ifndef TEXTEDIT_H_
#define TEXTEDIT_H_

#include <QTextEdit>
#include "dicwin.DictWrapper.h"
#include "dicwin.HttpRequester.h"

class QClipboard;
class QSystemTrayIcon;
class QMenu;
class QAction;

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
    
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayIconMenu;
    QAction* m_showAction;
    QAction* m_hideAction;
    QAction* m_quitAction;
    
    void createTrayActions();
    void createTrayIcon();
    void getWord(QString& qword, QString& result);
};
}
#endif
