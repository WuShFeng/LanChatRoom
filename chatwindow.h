#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
QT_BEGIN_NAMESPACE
namespace Ui {
class ChatWindow;
}
QT_END_NAMESPACE
class SendFile;
class ChatWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum MessageType{
        UDPMessage,
        UserJoin,
        UserLeft,
        SendFile
    };
    enum CastType{
        BroadCast,
        UniCast
    };
    ChatWindow(QWidget *parent = nullptr);
    ~ChatWindow();
    void closeEvent(QCloseEvent *event) override;
    bool sendMessage(MessageType messageType,QByteArray messageData="");

//setter&getter
public:
    void setUdpSendPort(const qint16 port);
    qint16 getUdpSendPort();
    void setUdpReceivePort(const qint16 port);
    qint16 getUdpReceivePort();
    QString getMessageFromEdit();
    QString getLocalIPAddress();
    QString getSelectedIPAddress();
    QString getSelectedUserName();
    bool isOnline(QString iPAddress);
    void setOnline(const QString userName,const QString hostName,const QString iPAddress);
    void setOffline(const QString iPAddress);
    CastType getCastType();

//slots
private slots:
    void on_sendMessageButton_clicked();
    void readUDPMessage();
    void on_switchCastTypeButton_clicked();
    void on_sendFileButton_clicked();
//variable
private:
    Ui::ChatWindow *ui;
    QUdpSocket *udpSocket;
    CastType castType;
    qint16 udpSendPort;
    qint16 udpReceivePort;
    QString localUserName;
    QString localHostName;
    QString localIPAddress;
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // CHATWINDOW_H
