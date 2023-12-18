//
// Created by WuShF on 2023/12/17.
//

#ifndef LANCHATROOM_FILESENDER_H
#define LANCHATROOM_FILESENDER_H

#include "filetransfer.h"
#include "../../components/UdpServices/UdpServer.h"

class FileSender : public FileTransfer{
public:

    FileSender(QList<QString> arguments);

    void selectFile() override;

    void transferFile() override;

    void transferData() override;

    void connected() override;

    void readMessage() override;
};


#endif //LANCHATROOM_FILESENDER_H
