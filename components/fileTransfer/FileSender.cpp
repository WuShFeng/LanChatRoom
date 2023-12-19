//
// Created by WuShF on 2023/12/17.
//

#include "FileSender.h"

FileSender::FileSender(QList<QString> arguments) {
    initUdpServer();
    QString castType = arguments[1];
    setTransferFileButtonEnabled(false);
    if (castType == "Broadcast") {
        this->castType = Broadcast;
    } else if (castType == "Unicast") {
        this->castType = Unicast;
        ipAddress = arguments[2];
    } else {
        this->close();
    }
}

void FileSender::selectFile() {
    filePath = QFileDialog::getOpenFileName(this);
    if (!filePath.isEmpty()) {
        if (localFile && localFile->isOpen()) {
            localFile->close();
        }
        localFile = new QFile(filePath);
        if (!localFile->open(QFile::ReadOnly)) {
            setStatusLabel(tr("无法读取文件"));
            return;
        }
        fileName = filePath.right(filePath.size() - filePath.lastIndexOf('/') - 1);
        fileSize = localFile->size();
        updateFileInfo(fileName, fileSize, filePath);
        setTransferFileButtonEnabled(true);
    }
}


void FileSender::transferFile() {
    localFile->seek(0);
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection, this, &FileSender::connected);
    tcpServer->listen(QHostAddress::Any);
    localPort = tcpServer->serverPort();
    QByteArray fileInfo;
    QDataStream out(&fileInfo, QIODevice::WriteOnly);
    if (castType == Broadcast) {
        out << localPort << fileName << fileSize;
        sendMessage(FileSend, castType, fileInfo);
    } else {
        out << ipAddress << localPort << fileName << fileSize;
        sendMessage(FileSend, castType, fileInfo);
    }
    setStatusLabel(tr("正在监听：") + QString::number(localPort));
    setSelectFileButtonEnabled(false);
    setTransferFileButtonEnabled(false);
}

void FileSender::transferData() {
    QByteArray data;
    data = localFile->read(1024 * 1024);
    if (data.size() == 0) {
        disConnected();
        return;
    }
    haveSendSize += data.size();
    notCountSize += data.size();
    updateProgress();
    tcpSocket->write(data);
#ifdef DEBUG
    updateFilePathInLabel(QString::number(tcpSocket->bytesToWrite()));
#endif
}

void FileSender::connected() {
    setStatusLabel(tr("已建立连接"));
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket, &QTcpSocket::bytesWritten, this, &FileSender::transferData);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &FileTransfer::disConnected);
#ifdef DEBUG
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, [=]() {
        updateFilePathInLabel(tcpSocket->errorString());
    });
#endif
    transferData();
    timer.start();
}

void FileSender::readMessage() {

}