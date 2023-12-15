#include "chatwindow.h"
#include <QApplication>
#include<QSharedMemory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QString sharedMemoryKey = "LanChatRoom";
    QSharedMemory sharedMemory(sharedMemoryKey);
    if(!sharedMemory.create(1)){
        return 1;
    }
    ChatWindow w;
    w.show();
    return a.exec();
}
