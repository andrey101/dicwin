#include <QApplication>
#include <QtDebug>

#include "khook.h"
#include "windows.h"
#include "dicwin.TextEdit.h"

const char* version="DICWIN VERSION 0.2";

int main(int argc, char *argv[])
{
    using namespace dicwin;
    
    QApplication app(argc, argv);
    qDebug("Starting DictWin app...");

    TextEdit textedit;

    QString hookText("Hook installed ");
    QString hookResult;

#ifndef NO_HOOK
    if (InstallKeyboardHook ())
        hookResult = "OK.";
    else
        hookResult = "NOK.";
#endif

    QString initText = hookText + hookResult;

    Qt::WindowFlags flags = Qt::Drawer;
    textedit.setWindowFlags(flags);
    
    textedit.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    textedit.setReadOnly(true);
    textedit.setWindowTitle("Dicwin 1.0");
    textedit.setText(initText);
    textedit.show();

    app.exec();
    return 0;
}
