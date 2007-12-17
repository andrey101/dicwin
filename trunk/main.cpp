#include <QApplication>
#include <QtDebug>
#include <QVBoxLayout>

#include "hook.h"
#include "windows.h"
#include "dicwin.TextEdit.h"

const char* version="DICWIN VERSION 0.3";

using namespace dicwin;

//class Preview : public QWidget
//{
//public:
//	Preview(TextEdit* t) : QWidget(), textedit(t) 
//	{
//	    //textedit.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//	    textedit->setReadOnly(true);
//	    
//	    QVBoxLayout *layout = new QVBoxLayout;
//	    layout->addWidget(textedit);
//	    setLayout(layout);
//	    setWindowTitle("Dicwin 1.0");
//	};
//	
//private:
//	TextEdit* textedit; 
//};

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
    //flags |= Qt::FramelessWindowHint;
    
    TextEdit* textedit = new TextEdit();
    textedit->setReadOnly(true);
    textedit->setWindowTitle("Dicwin 1.0");
    textedit->setWindowFlags(flags);
    textedit->show();
    
//    Preview preview(textEdit);
//    preview.setWindowFlags(flags);
//    preview.show();

    app.exec();
    return 0;
}
