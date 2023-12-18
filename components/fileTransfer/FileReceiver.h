//
// Created by WuShF on 2023/12/17.
//

#ifndef LANCHATROOM_FILERECEIVER_H
#define LANCHATROOM_FILERECEIVER_H
#include "filetransfer.h"

class FileReceiver: public FileTransfer{
public:

    explicit FileReceiver(QStringList list);
    void selectFile() override;
    void connected() override;
    void transferFile() override;
    void readMessage() override;
    void transferData() override;
};


#endif //LANCHATROOM_FILERECEIVER_H
