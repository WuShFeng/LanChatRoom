//
// Created by WuShF on 2023/12/17.
//
#include <QApplication>
#include "FileSender.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return argc;
    }
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    FileSender sender(a.arguments());
    sender.show();
    return QApplication::exec();
}
