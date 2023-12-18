//
// Created by WuShF on 2023/12/17.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FileTransfer.h" resolved

#include "filetransfer.h"
#include "ui_FileTransfer.h"


FileTransfer::FileTransfer(QWidget *parent) :
         ui(new Ui::FileTransfer) {
    ui->setupUi(this);
    tcpServer = NULL;
    tcpSocket = NULL;
    localFile = NULL;
    fileSize = 0;
    haveSendSize = 0;
    notCountSize = 0;
    localPort = 0;
    serverPort = 0;
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    connect(ui->selectFileButton, &QPushButton::clicked, this, &FileTransfer::selectFile);
    connect(ui->transferFileButton, &QPushButton::clicked, this, &FileTransfer::transferFile);
    connect(ui->cancelButton, &QPushButton::clicked, this, [=]() {
        this->close();
    });
}

FileTransfer::~FileTransfer() {
    delete ui;
    if(tcpSocket&&tcpSocket->isOpen()){
        tcpSocket->close();
    }
    if(tcpServer&&tcpServer->isListening()){
        tcpServer->close();
    }
    if(localFile&&localFile->isOpen()){
        localFile->close();
    }
}

void FileTransfer::updateProgress() {
    ui->progressBar->setValue(haveSendSize * 100 / fileSize);
#ifdef DEBUG
    static int a=0;
    setStatusLabel("times: "+QString::number(++a)+" haveSend: "+QString::number(haveSendSize)+" fileSize: "+QString::number(fileSize));
#else
    qint64 time = timer.elapsed();
    if (time > 500) {
        double speed = notCountSize / 1024 / 1.024 / time;
        ui->statusLabel->setText(tr("传输速度：") + QString::number(speed) + tr("MB/s"));
        notCountSize = 0;
        timer.restart();
    }
#endif
}

void FileTransfer::disConnected() {
#ifndef DEBUG
    tcpSocket->close();
    if (haveSendSize == fileSize) {
        ui->statusLabel->setText(tr("传输已完成"));
    } else {
        ui->statusLabel->setText(tr("连接已中断：")+QString::number(haveSendSize)+"/"+QString::number(fileSize));
    }
#endif

}

void FileTransfer::updateFileInfo(QString fileName, qint64 fileSize, QString filePath) {
    ui->fileNameLabel->setText(tr("文件名：") + fileName);
    updateFilePathInLabel(filePath);
    fileSize < 1024 ? ui->fileSizeLabel->setText(tr("文件大小：%1B").arg(fileSize))
                    : fileSize < 1024 * 1024 ? ui->fileSizeLabel->setText(tr("文件大小：%1KB").arg(fileSize / 1024.0))
                                             : fileSize < 1024 * 1024 * 1024 ? ui->fileSizeLabel->setText(
                    tr("文件大小：%1MB").arg(fileSize / 1024 / 1024.0))
                                                                             : ui->fileSizeLabel->setText(
                            tr("文件大小：%1GB").arg(fileSize / 1024 / 1024 / 1024.0));
}

void FileTransfer::setSelectFileButtonEnabled(bool status) {
    ui->selectFileButton->setEnabled(status);
}

void FileTransfer::setTransferFileButtonEnabled(bool status) {
    ui->transferFileButton->setEnabled(status);
}

void FileTransfer::setStatusLabel(QString message) {
    ui->statusLabel->setText(message);
}

void FileTransfer::updateFilePathInLabel(QString filePath) {
    ui->filePathLabel->setText(tr("文件位置：") + filePath);
}
