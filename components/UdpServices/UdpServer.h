//
// Created by WuShF on 2023/12/18.
//

#ifndef LANCHATROOM_UDPSERVER_H
#define LANCHATROOM_UDPSERVER_H
#include "QString"
#include "QUdpSocket"
#include "QHostInfo"
#include "QNetworkInterface"
#include "QMainWindow"
class UdpServer : public QMainWindow{
public:
    enum MessageType {
        MessageSend,
        UserJoin,
        UserLeft,
        FileSend
    };
    enum CastType {
        Broadcast,
        Unicast,
        NotDefine
    };

    explicit UdpServer();
    const QString getLocalUserName() const;

    const QString getLocalHostName() const;

    const QString getLocalIpAddress() const;

    QString localUserName;
    QString localHostName;
    QString localIpAddress;
    QUdpSocket *udpSocket{};
    qint16 port;
    CastType castType;
public slots:

    void sendMessage(MessageType messageType, CastType castType = NotDefine, QByteArray data = "");

    virtual void readMessage() = 0;
};


#endif //LANCHATROOM_UDPSERVER_H
