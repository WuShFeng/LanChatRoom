//
// Created by WuShF on 2023/12/17.
//

#ifndef LANCHATROOM_FILETRANSFER_H
#define LANCHATROOM_FILETRANSFER_H

#include "QElapsedTimer"
#include "QWidget"
#include "QFileDialog"
#include "QFile"
#include "QStandardPaths"
#include "QDateTime"
#include "QTcpServer"
#include "QTcpSocket"
#include "QFileDialog"
#include "QNetworkProxy"
#include "../../components/UdpServices/UdpServer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FileTransfer; }
QT_END_NAMESPACE

class FileTransfer : public UdpServer {
Q_OBJECT

public:
    explicit FileTransfer(QWidget *parent = nullptr);

    ~FileTransfer() override;

    void setStatusLabel(QString message);

    void setSelectFileButtonEnabled(bool status);

    void setTransferFileButtonEnabled(bool status);

    void updateFilePathInLabel(QString filePath);

    void updateProgress();

    void updateFileInfo(QString fileName, qint64 fileSize, QString filePath);

public slots:

    void disConnected();

    virtual void connected() = 0;

    virtual void selectFile() = 0;

    virtual void transferFile() = 0;

    virtual void transferData() = 0;

protected:
    Ui::FileTransfer *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QElapsedTimer timer;
    QString ipAddress;
    QString filePath;
    QFile *localFile;
    QString fileName;
    qint64 fileSize;
    qint64 haveSendSize;
    qint64 notCountSize;
    qint16 localPort;//tcpServer->listen
    qint16 serverPort;//tcpSocket->connect
};


#endif //LANCHATROOM_FILETRANSFER_H
