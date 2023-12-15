#include "chatwindow.h"
#include "./ui_chatwindow.h"
#include<QHostInfo>
#include<QNetworkInterface>
#include<QDateTime>
#include<QKeyEvent>
#include"sendfile.h"
#include"receivefile.h"
ChatWindow::ChatWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ChatWindow)
{
    ui->setupUi(this);
    udpReceivePort=5555;
    udpSendPort=5555;
    castType=BroadCast;
    localUserName=QString::fromLocal8Bit(qgetenv("USERNAME"));
    localHostName=QHostInfo::localHostName();
    localIPAddress=getLocalIPAddress();
    ui->messageTextEdit->installEventFilter(this);
    udpSocket=new QUdpSocket(this);
    udpSocket->bind(udpReceivePort,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    connect(udpSocket,&QUdpSocket::readyRead,this,&ChatWindow::readUDPMessage);
    sendMessage(UserJoin);
}

ChatWindow::~ChatWindow()
{
    delete ui;
    delete udpSocket;
}
void ChatWindow::closeEvent(QCloseEvent *event){
    sendMessage(UserLeft);
    QMainWindow::closeEvent(event);
}
//setter&getter
void ChatWindow::setUdpSendPort(const qint16 port){
    this->udpSendPort=port;
}
qint16 ChatWindow::getUdpSendPort(){
    return this->udpSendPort;
}
void ChatWindow::setUdpReceivePort(const qint16 port){
    this->udpReceivePort=port;
}
qint16 ChatWindow::getUdpReceivePort(){
    return this->udpReceivePort;
}
QString ChatWindow::getMessageFromEdit(){
    return ui->messageTextEdit->toHtml();
}
QString ChatWindow::getLocalIPAddress(){
    if(!localIPAddress.isEmpty()){
        return localIPAddress;
    }
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
            !interface.flags().testFlag(QNetworkInterface::IsLoopBack) &&
            interface.type()!=QNetworkInterface::Virtual&&
            !interface.humanReadableName().contains(tr("vEthernet"))) {
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            foreach (QNetworkAddressEntry entry, entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    return entry.ip().toString();
                }
            }
        }
    }
    return tr("unknown");
}
QString ChatWindow::getSelectedIPAddress(){
    int selectedRow=ui->userTableWidget->currentRow();
    if(selectedRow==-1){
        return tr("unknown");
    }
    QString selectedIPAddress=ui->userTableWidget->item(selectedRow,2)->text();
    return selectedIPAddress;
}

QString ChatWindow::getSelectedUserName()
{
    int selectedRow=ui->userTableWidget->currentRow();
    if(selectedRow==-1){
        return tr("unknown");
    }
    QString selectedUserName=ui->userTableWidget->item(selectedRow,0)->text();
    return selectedUserName;
}
bool ChatWindow::isOnline(QString iPAddress){
    auto users= ui->userTableWidget->findItems(iPAddress,Qt::MatchExactly);
    if(users.isEmpty()){
        return false;
    }
    return true;
}
void ChatWindow::setOnline(const QString userName, const QString hostName, const QString iPAddress){
    QTableWidgetItem*user=new QTableWidgetItem(userName);
    QTableWidgetItem*host=new QTableWidgetItem(hostName);
    QTableWidgetItem*ip=new QTableWidgetItem(iPAddress);
    ui->userTableWidget->insertRow(0);
    ui->userTableWidget->setItem(0,0,user);
    ui->userTableWidget->setItem(0,1,host);
    ui->userTableWidget->setItem(0,2,ip);
    ui->messageTextBrowser->setTextColor(Qt::blue);
    ui->messageTextBrowser->append(tr("%1  %2").arg(userName,QDateTime::currentDateTime().toString()));
    ui->messageTextBrowser->setTextColor(Qt::gray);
    ui->messageTextBrowser->append(tr("加入聊天室"));
}
void ChatWindow::setOffline(const QString iPAddress){
    auto findItems= ui->userTableWidget->findItems(iPAddress,Qt::MatchExactly);
    if(findItems.isEmpty()){
        return;
    }
    int rowNum=findItems.first()->row();
    QString offlineUserName=ui->userTableWidget->item(rowNum,0)->text();
    ui->messageTextBrowser->setTextColor(Qt::blue);
    ui->messageTextBrowser->append(tr("%1  %2").arg(offlineUserName,QDateTime::currentDateTime().toString()));
    ui->messageTextBrowser->setTextColor(Qt::gray);
    ui->messageTextBrowser->append(tr("加入聊天室"));
    ui->userTableWidget->removeRow(rowNum);
}
ChatWindow::CastType ChatWindow::getCastType(){
    return this->castType;
}
//slots
void ChatWindow::on_sendMessageButton_clicked()
{
    QString message=ui->messageTextEdit->toPlainText();
    if(message.isEmpty()){
        ui->statusBar->showMessage(tr("输入消息不能为空"));
        return;
    }
    sendMessage(UDPMessage);
}
void ChatWindow::readUDPMessage(){
    QByteArray data;
    data.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(data.data(),data.size());
    QDataStream in(&data,QIODevice::ReadOnly);
    MessageType messageType;
    CastType castType;
    QString serverUserName;
    QString serverHostName;
    QString serverIPAddress;
    in>>messageType>>castType>>serverUserName>>serverHostName>>serverIPAddress;
    if(!isOnline(serverIPAddress)){
        sendMessage(UserJoin);
        setOnline(serverUserName,serverHostName,serverIPAddress);
    }
    switch (messageType) {
    case UDPMessage:{
        QString message;
        in>>message;
        if(castType==BroadCast){
            //broadcast
            ui->messageTextBrowser->setTextColor(Qt::blue);
            ui->messageTextBrowser->append(tr("%1  %2").arg(serverUserName,QDateTime::currentDateTime().toString()));
            ui->messageTextBrowser->setTextColor(Qt::black);
            ui->messageTextBrowser->append(tr("%1").arg(message));
        }else{
            //unicast
            ui->messageTextBrowser->setTextColor(Qt::blue);
            ui->messageTextBrowser->append(tr("%1  %2").arg(serverUserName,QDateTime::currentDateTime().toString()));
            ui->messageTextBrowser->setTextColor(Qt::gray);
            ui->messageTextBrowser->append(tr("私发给你一条消息"));
            ui->messageTextBrowser->setTextColor(Qt::black);
            ui->messageTextBrowser->append(tr("%1").arg(message));
        }
        break;
    }
    case UserLeft:{
        if(serverIPAddress==localIPAddress){
            return;
        }
        setOffline(serverIPAddress);
        ui->messageTextBrowser->setTextColor(Qt::blue);
        ui->messageTextBrowser->append(tr("%1  %2").arg(serverUserName,QDateTime::currentDateTime().toString()));
        ui->messageTextBrowser->setTextColor(Qt::gray);
        ui->messageTextBrowser->append(serverUserName+tr("离开了"));
        break;
    }
    case SendFile:{
        QString fileName;
        qint64 fileSize;
        QString address;
        qint16 port;
        in>>fileName>>fileSize>>port;
        ui->messageTextBrowser->setTextColor(Qt::blue);
        ui->messageTextBrowser->append(tr("%1  %2").arg(serverUserName,QDateTime::currentDateTime().toString()));
        ui->messageTextBrowser->setTextColor(Qt::gray);
        if(castType==BroadCast){
            ui->messageTextBrowser->append(serverUserName+tr("广播了一个文件  ")+fileName);
            if(serverIPAddress==localIPAddress){
                return;
            }
        }else{
            ui->messageTextBrowser->append(serverUserName+tr("私发给你一个文件  ")+fileName);
        }
        ReceiveFile receiveFile;
        receiveFile.setFileName(fileName);
        receiveFile.setFileSize(fileSize);
        receiveFile.setPort(port);
        receiveFile.setIpAddress(serverIPAddress);
        receiveFile.show();
        receiveFile.exec();
        break;
    }
    }
}
//public method
bool ChatWindow::sendMessage(MessageType messageType,QByteArray messageData){
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    //send local info
    out<<messageType<<castType<<localUserName<<localHostName<<localIPAddress;
    switch (messageType) {
    case UDPMessage:{
        QString message=getMessageFromEdit();
        out<<message;
        if(castType==BroadCast){
            //broadcast
            udpSocket->writeDatagram(data,data.size(),QHostAddress::Broadcast,udpSendPort);
        }else{
            //unicast
            QString selectedIPAddress=getSelectedIPAddress();
            if(selectedIPAddress=="unknown"){
                ui->statusBar->showMessage(tr("先选择要发送的用户"));
                return false;
            }
            if(selectedIPAddress!=localIPAddress){
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(tr("%1  %2").arg(localUserName,QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::gray);
                ui->messageTextBrowser->append(tr("你私发给%1一条消息").arg(getSelectedUserName()));
                ui->messageTextBrowser->setTextColor(Qt::black);
                ui->messageTextBrowser->append(tr("%1").arg(message));
            }
            udpSocket->writeDatagram(data,data.size(),QHostAddress(selectedIPAddress),udpSendPort);
        }
        ui->messageTextEdit->clear();
        break;
    }
    case UserJoin:{
        //broadcast
        udpSocket->writeDatagram(data,data.size(),QHostAddress::Broadcast,udpSendPort);
        break;
    }
    case UserLeft:{
        //broadcast
        udpSocket->writeDatagram(data,data.size(),QHostAddress::Broadcast,udpSendPort);
        break;
    }
    case SendFile:{
        data+=messageData;
        if(castType==BroadCast){
            //broadcast
            udpSocket->writeDatagram(data,data.size(),QHostAddress::Broadcast,udpSendPort);
        }else{
            //unicast
            QString selectedIPAddress=getSelectedIPAddress();
            if(selectedIPAddress=="unknown"){
                ui->statusBar->showMessage(tr("先选择要发送的用户"));
                return false;
            }
            udpSocket->writeDatagram(data,data.size(),QHostAddress(selectedIPAddress),udpSendPort);
        }
    }
    }
    return true;
}

void ChatWindow::on_switchCastTypeButton_clicked()
{
    castType==BroadCast?castType=UniCast:castType=BroadCast;
    if(castType==BroadCast){
        ui->switchCastTypeButton->setText(tr("切换消息发送方式（目前：局域网多播）"));
    }else{
        ui->switchCastTypeButton->setText(tr("切换消息发送方式（目前：一对一单播）"));
    }
}


void ChatWindow::on_sendFileButton_clicked()
{
    class SendFile sendfile;
    sendfile.setChatWindow(this);
    sendfile.show();
    sendfile.exec();
}

bool ChatWindow::eventFilter(QObject *obj, QEvent *event)
{
    // qDebug()<<"event->type"<<event->type();
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        // qDebug()<<"keyEvent->key"<<keyEvent->key();
        if (keyEvent->key() == Qt::Key_Enter||keyEvent->key()==Qt::Key_Return) {
            if (keyEvent->modifiers() & Qt::ControlModifier) {
                ui->messageTextEdit->insertPlainText("\n");
            } else {
                QTextCursor cursor = ui->messageTextEdit->textCursor();
                int cursorPos = cursor.position();
                if (cursorPos >= 0 && cursorPos <= ui->messageTextEdit->toPlainText().length()) {
                    this->on_sendMessageButton_clicked();
                }
            }
            return true;
        }
    }
    return false;
}


