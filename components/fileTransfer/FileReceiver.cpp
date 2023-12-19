//
// Created by WuShF on 2023/12/17.
//

#include "FileReceiver.h"

FileReceiver::FileReceiver(QStringList list) {
    ipAddress = list[1];
    serverPort = list[2].toInt();
    fileName = list[3];
    fileSize = list[4].toLongLong();
    QString downloadLocation = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    filePath = downloadLocation + '/' + fileName;
    QFile file(filePath);
    if (file.exists()) {
        fileName = QString::number(QDateTime::currentSecsSinceEpoch()) + '-' + fileName;
        filePath = downloadLocation + '/' + fileName;
    }
    localFile = new QFile(filePath);
    updateFileInfo(fileName, fileSize, filePath);
    if (!localFile->open(QIODevice::WriteOnly)) {
        setStatusLabel(tr("文件创建失败"));
        return;
    }
}

void FileReceiver::selectFile() {
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile(fileName);
    dialog.setWindowTitle(tr("选择文件保存位置"));
    if (dialog.exec()) {
        QString selectedFilePath = dialog.selectedFiles()[0];
        filePath = selectedFilePath;
        setTransferFileButtonEnabled(true);
        updateFilePathInLabel(filePath);
    }

}

void FileReceiver::transferFile() {
    localFile->seek(0);
    setStatusLabel(tr("正在尝试建立连接-") + ipAddress + ":" + QString::number(serverPort));
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &FileReceiver::connected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &FileTransfer::disConnected);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &FileReceiver::transferData);
#ifdef DEBUG
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, [=]() {
        updateFilePathInLabel(tcpSocket->errorString());
    });
#endif
    tcpSocket->connectToHost(QHostAddress(ipAddress), serverPort);
    setSelectFileButtonEnabled(false);
    setTransferFileButtonEnabled(false);
}

void FileReceiver::transferData() {
    QByteArray data;
    data = tcpSocket->read(1024 * 1024);
    localFile->write(data);
    haveSendSize += data.size();
    notCountSize += data.size();
    updateProgress();
}

void FileReceiver::connected() {
    setStatusLabel(tr("已建立连接"));
    timer.start();
}

void FileReceiver::readMessage() {

}

FileReceiver::~FileReceiver() {
    if (haveSendSize != fileSize) {
        localFile->remove();
    }
}


