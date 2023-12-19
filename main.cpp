#include <QApplication>
#include <QSharedMemory>
#include "chatroom.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    const QString sharedMemoryKey = "LanChatRoom";
    QSharedMemory sharedMemory(sharedMemoryKey);
    if (!sharedMemory.create(1)) {
        return 1;
    }
    ChatRoom chatRoom;
    chatRoom.show();
    return QApplication::exec();
}
