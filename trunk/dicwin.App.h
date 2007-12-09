#ifndef DICWIN_APP_H
#define DICWIN_APP_H

#include <QMessageBox>
#include "dicwin.DictWrapper.h"

class QClipboard;

namespace dicwin {

class DictWrapper;

class App : public QMessageBox
{
    Q_OBJECT

public:
    App(QWidget *parent = 0);

public slots:
    void showClipText();

private:
    QClipboard* m_clipboard;
    DictWrapper* m_dictWrapper;
    void getWord(QString& qword, QString& result);
};
}
#endif
