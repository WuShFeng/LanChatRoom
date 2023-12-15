#include "receivefile.h"
#include "ui_receivefile.h"
#include<QFileDialog>
#include<QNetworkProxy>
#include<QTcpSocket>
#include<QFile>
ReceiveFile::ReceiveFile(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
    tcpSocket=NULL;
    localFile=NULL;
    haveSendSize=0;
    notCountSize=0;
    fileSize=0;
    ui->receiveFileButton->setEnabled(false);
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
    localFile->deleteLater();
    tcpSocket->deleteLater();
}

void ReceiveFile::showEvent(QShowEvent *event)
{
    ui->fileNameLabel->setText(tr("文件名：")+fileName);
    ui->senderInfoLabel->setText(tr("发送端信息：")+ipAddress+":"+QString::number(port));
    fileSize<1024?ui->fileSizeLabel->setText(tr("文件大小：%1B").arg(fileSize))
        :fileSize<1024*1024?ui->fileSizeLabel->setText(tr("文件大小：%1KB").arg(fileSize/1024.0))
            :fileSize<1024*1024*1024?ui->fileSizeLabel->setText(tr("文件大小：%1MB").arg(fileSize/1024/1024.0))
                :ui->fileSizeLabel->setText(tr("文件大小：%1GB").arg(fileSize/1024/1024/1024.0));
    QDialog::showEvent(event);
}

void ReceiveFile::closeEvent(QCloseEvent *event)
{
    if(tcpSocket&&tcpSocket->isOpen()){
        tcpSocket->close();
    }
    if(haveSendSize!=fileSize){
        if(localFile&&localFile->isOpen()){
            localFile->close();
            localFile->remove();
        }
    }
    this->close();
    // QDialog::closeEvent(event);
}
//setter&getter
void ReceiveFile::setFileName(QString fileName){
    this->fileName=fileName;
}
void ReceiveFile::setFileSize(qint64 fileSize){
    this->fileSize=fileSize;
}
void ReceiveFile::setPort(qint16 port){
    this->port=port;
}
void ReceiveFile::setIpAddress(const QString &newIpAddress)
{
    ipAddress = newIpAddress;
}
void ReceiveFile::getData()
{
    QByteArray data;
    data=tcpSocket->read(1024*1024);
    localFile->write(data);
    haveSendSize+=data.size();
    notCountSize+=data.size();
    qint64 time=timer.elapsed();
    if(time>500){
        qfloat16 speed=notCountSize/1024/1.024/time;
        ui->statusLabel->setText(tr("传输速度：%1MB/s").arg(speed));
        notCountSize=0;
        timer.restart();
    }
    ui->progressBar->setValue(haveSendSize*100/fileSize);
}
//slots
void ReceiveFile::connected()
{
    ui->statusLabel->setText(tr("已建立连接"));
    ui->selectFilePathButton->setEnabled(false);
    ui->receiveFileButton->setEnabled(false);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&ReceiveFile::getData);
    timer.start();
}
void ReceiveFile::on_selectFilePathButton_clicked()
{
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.selectFile(fileName);
    dialog.setWindowTitle(tr("选择文件保存位置"));
    if(dialog.exec()){
        QString selectedFilePath=dialog.selectedFiles()[0];
        filePath=selectedFilePath;
        ui->receiveFileButton->setEnabled(true);
        ui->filePathLabel->setText(tr("文件路径：")+filePath);
    }
}
void ReceiveFile::on_receiveFileButton_clicked()
{
    localFile=new QFile(filePath);
    if(!localFile->open(QIODevice::WriteOnly)){
        ui->statusLabel->setText(tr("文件创建失败"));
        return;
    }
    tcpSocket=new QTcpSocket(this);
    connect(tcpSocket,&QTcpSocket::connected,this,&ReceiveFile::connected);
    connect(tcpSocket,&QTcpSocket::errorOccurred,this,[=](){
        qDebug()<<tcpSocket->errorString();
        ui->statusLabel->setText(tcpSocket->errorString());
    });
    connect(tcpSocket,&QTcpSocket::disconnected,this,&ReceiveFile::disconnected);
    tcpSocket->connectToHost(QHostAddress(ipAddress),port);
    ui->statusLabel->setText(tr("正在尝试建立连接"));
    ui->selectFilePathButton->setEnabled(false);
    ui->receiveFileButton->setEnabled(false);
}
void ReceiveFile::disconnected()
{
    if(haveSendSize==fileSize){
        ui->statusLabel->setText(tr("传输已完成"));
    }else{
        ui->statusLabel->setText(tr("传输已中断"));
    }
}


void ReceiveFile::on_cancelButton_clicked()
{
    closeEvent(NULL);
}

