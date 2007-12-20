#include <QApplication>
#include <QtDebug>
#include <QVBoxLayout>

#include "hook.h"
#include "windows.h"
#include "dicwin.TextEdit.h"

const char* version="DICWIN VERSION 0.4";

using namespace dicwin;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug("Starting DictWin app...");
    
    QString hookText("Hook installed ");
    QString hookResult;

#ifndef NO_HOOK
    if (InstallHook (Keyboard))
        hookResult = "OK.";
    else
        hookResult = "NOK.";
#endif
    QString initText = hookText + hookResult;

    Qt::WindowFlags flags = Qt::Window;
    //flags |= Qt::WindowTitleHint;
    
    TextEdit* textedit = new TextEdit();
    textedit->setReadOnly(true);
    //textedit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);    
    textedit->setWindowTitle("Dicwin 1.0");
    textedit->setWindowFlags(flags);
    //textedit->show();

    app.exec();
    return 0;
}
