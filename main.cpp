#include <QApplication>
#include <QtDebug>

#include "khook.h"
#include "windows.h"
#include "dicwin.App.h"

const char* version="DICWIN VERSION 0.1";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qDebug("Starting DictWin app...");

    dicwin::App dicwinApp;

    QString hookText("Hook installed ");
    QString hookResult;

#ifndef NO_HOOK
    if (InstallKeyboardHook ())
        hookResult = "OK.";
    else
        hookResult = "NOK.";
#endif

    for (int i=0; i < 100; i++) {
        int a = a*i;
    }
    QString initText = hookText + hookResult;

    dicwinApp.setWindowTitle("Qt Tom Test Dialog");
    dicwinApp.setGeometry(20, 30, 200, 60);
    dicwinApp.setText(initText);
    dicwinApp.show();

    app.exec();
    return 0;
}
