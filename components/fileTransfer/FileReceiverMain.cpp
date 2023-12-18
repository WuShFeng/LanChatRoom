//
// Created by WuShF on 2023/12/17.
//
#include <QApplication>
#include"FileReceiver.h"
int main(int argc, char *argv[]) {
    if(argc<5){
        return argc;
    }
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    FileReceiver fileReceiver(a.arguments());
    fileReceiver.show();
    return QApplication::exec();
}
