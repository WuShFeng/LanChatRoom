#include "chatwindow.h"
#include <QApplication>
#include<QResource>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ChatWindow w;
    w.show();
    return a.exec();
}
