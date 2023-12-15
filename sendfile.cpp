#include "sendfile.h"
#include "ui_sendfile.h"
#include"chatwindow.h"
#include<QTcpServer>
#include<QFile>
#include<QFileDialog>
#include<QTcpSocket>
SendFile::SendFile(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SendFile)
{
    ui->setupUi(this);
    tcpServer=new QTcpServer();
    localFile=NULL;
    tcpSocket=NULL;
    tcpServer->listen(QHostAddress::LocalHost);
    port=tcpServer->serverPort();
    tcpServer->close();
    haveSendSize=0;
    notCountSize=0;
    fileSize=0;
    ui->sendFileButton->setEnabled(false);
}

SendFile::~SendFile()
{
    delete ui;
    disconnect(tcpServer,&QTcpServer::newConnection,this,&SendFile::newConnection);
    localFile->deleteLater();
    tcpServer->deleteLater();
    tcpSocket->deleteLater();
}
//setter&getter
void SendFile::setChatWindow(ChatWindow *chatWindow){
    this->chatWindow=chatWindow;
}

//slots
void SendFile::closeEvent(QCloseEvent *event){
    if(tcpServer&&tcpServer->isListening()){
        tcpServer->close();
    }
    if(localFile&&localFile->isOpen()){
        localFile->close();
    }
    if(tcpSocket&&tcpSocket->isOpen()){
        tcpSocket->close();
    }
    this->close();
    // QDialog::closeEvent(event);
}

void SendFile::on_selectFileButton_clicked()
{
    filePath=QFileDialog::getOpenFileName(this);
    if(!filePath.isEmpty()){
        if(tcpServer&&tcpServer->isListening()){
            tcpServer->close();
        }
        if(localFile&&localFile->isOpen()){
            localFile->close();
        }
        localFile=new QFile(filePath);
        if(!localFile->open(QFile::ReadOnly)){
            ui->statusLabel->setText(tr("无法读取文件"));
            return;
        }
        fileName=filePath.right(filePath.size()-filePath.lastIndexOf('/')-1);
        ui->sendFileButton->setEnabled(true);
        ui->fileNameLabel->setText(tr("文件名：")+fileName);
        ui->filePathLabel->setText(tr("文件路径：")+filePath);
        fileSize=localFile->size();
        fileSize<1024?ui->fileSizeLabel->setText(tr("文件大小：%1B").arg(fileSize))
            :fileSize<1024*1024?ui->fileSizeLabel->setText(tr("文件大小：%1KB").arg(fileSize/1024.0))
                :fileSize<1024*1024*1024?ui->fileSizeLabel->setText(tr("文件大小：%1MB").arg(fileSize/1024/1024.0))
                    :ui->fileSizeLabel->setText(tr("文件大小：%1GB").arg(fileSize/1024/1024/1024.0));
    }
}


void SendFile::on_cancelButton_clicked()
{
    closeEvent(nullptr);
}


void SendFile::on_sendFileButton_clicked()
{
    QByteArray info;
    QDataStream out(&info,QIODevice::WriteOnly);
    out<<fileName<<fileSize<<port;
    if(chatWindow->getCastType()==ChatWindow::CastType::BroadCast){
        //broadcast
        tcpServer->listen(QHostAddress::Any,port);
    }else{
        //unicast
        tcpServer->listen(QHostAddress(chatWindow->getSelectedIPAddress()),port);
    }
    if(chatWindow->sendMessage(ChatWindow::SendFile,info)){
        ui->selectFileButton->setEnabled(false);
        ui->sendFileButton->setEnabled(false);
        ui->statusLabel->setText(tr("等待客户端接收文件"));
        connect(tcpServer,&QTcpServer::newConnection,this,&SendFile::newConnection);
    }
}

void SendFile::newConnection(){
    localFile->seek(0);
    ui->statusLabel->setText(tr("已建立连接"));
    ui->selectFileButton->setEnabled(false);
    ui->sendFileButton->setEnabled(false);
    tcpSocket=tcpServer->nextPendingConnection();
    connect(tcpSocket,&QTcpSocket::bytesWritten,this,&SendFile::sendData);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&SendFile::disConnected);
    disconnect(tcpServer,&QTcpServer::newConnection,this,&SendFile::newConnection);
    timer.start();
    sendData();
}

void SendFile::disConnected()
{
    ui->statusLabel->setText(tr("客户端中断连接"));
    tcpSocket->close();
}

void SendFile::sendData()
{
    qApp->processEvents();
    QByteArray data;
    data=localFile->read(1024*1024);
    if(data.size()==0){
        tcpSocket->close();
        if(haveSendSize==fileSize){
            ui->statusLabel->setText(tr("传输已完成"));
        }else{
            ui->statusLabel->setText(tr("传输异常，可能存在数据丢失"));
        }
        return;
    }
    haveSendSize+=data.size();
    notCountSize+=data.size();
    qint64 time=timer.elapsed();
    if(time>500){
        qfloat16 speed=notCountSize/1024/1.024/time;
        ui->speedLabel->setText(tr("传输速度：%1MB/s").arg(speed));
        notCountSize=0;
        timer.restart();
    }
    ui->progressBar->setValue(haveSendSize*100/fileSize);
    tcpSocket->write(data);
}
