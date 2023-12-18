//
// Created by WuShF on 2023/12/18.
//
#include "UdpServer.h"
UdpServer::UdpServer() {
    localUserName = getLocalUserName();
    localHostName = getLocalHostName();
    localIpAddress = getLocalIpAddress();
    port=5555;
    castType=Broadcast;
    udpSocket=new QUdpSocket(this);
    connect(udpSocket,&QUdpSocket::readyRead,this,&UdpServer::readMessage);
    udpSocket->bind(port,QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
    sendMessage(UserJoin);
}
const QString UdpServer::getLocalUserName() const {
    if (!localUserName.isEmpty()) {
        return localUserName;
    }
    return QString::fromLocal8Bit(qgetenv("USERNAME"));
}

const QString UdpServer::getLocalHostName() const {
    if (!localHostName.isEmpty()) {
        return localHostName;
    }
    return QHostInfo::localHostName();
}

const QString UdpServer::getLocalIpAddress() const {
    if (!localIpAddress.isEmpty()) {
        return localIpAddress;
    }
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
            foreach (QNetworkInterface interface, interfaces) {
            if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
                !
                        interface.flags().testFlag(QNetworkInterface::IsLoopBack) &&
                interface.type() != QNetworkInterface::Virtual &&
                !
                        interface.humanReadableName().contains(tr("vEthernet"))) {
                QList<QNetworkAddressEntry> entries =
                        interface.addressEntries();
                        foreach (QNetworkAddressEntry entry, entries) {
                        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                            return entry.ip().toString();
                        }
                    }
            }
        }
    return tr("unknown");
}


void UdpServer::sendMessage(UdpServer::MessageType messageType, UdpServer::CastType castType, QByteArray data) {
    QByteArray messageData;
    QDataStream out(&messageData, QIODevice::WriteOnly);
    if (castType == NotDefine) {
        castType = this->castType;
    }
    out << messageType << castType << localUserName << localHostName << localIpAddress;
    switch (messageType) {
        case MessageSend: {
        }
        case FileSend: {
            messageData += data;
            if(castType==Broadcast){
                udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress::Broadcast, port);
            }else{
                QString serverIpAddress;
                QDataStream in(&data,QIODevice::ReadOnly);
                in>>serverIpAddress;
                udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress(serverIpAddress), port);
            }
            break;
        }
        case UserJoin: {
        }
        case UserLeft: {
            udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress::Broadcast, port);
            break;
        }
    }
}


