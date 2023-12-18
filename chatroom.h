//
// Created by WuShF on 2023/12/17.
//

#ifndef LANCHATROOM_CHATROOM_H
#define LANCHATROOM_CHATROOM_H

#include <QMainWindow>
#include <QElapsedTimer>
#include <QTimer>
#include <QTableWidget>
#include "components/UdpServices/UdpServer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ChatRoom; }
QT_END_NAMESPACE
class ChatRoom : public UdpServer {
Q_OBJECT

public:
    explicit ChatRoom(QWidget *parent = nullptr);

    ~ChatRoom() override;

    bool eventFilter(QObject *obj, QEvent *event) override;

    bool isOnline(QString ipAddress);

    void setOnline(const QString userName, const QString hostName, const QString iPAddress);

    bool setOffine(const QString iPAddress);

    void sendMessageFromTextEdit();

    QList<QTableWidgetItem *> getSelectedUserInfo();

public slots:

    void switchCastType();
    void sendMessage(MessageType messageType, CastType castType = NotDefine, QByteArray data = "");
    void readMessage() override;
    void sendFile();

private:
    Ui::ChatRoom *ui;
    QTimer timer;
};


#endif //LANCHATROOM_CHATROOM_H
