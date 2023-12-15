#ifndef SENDFILE_H
#define SENDFILE_H
#include<QElapsedTimer>
#include <QDialog>
class ChatWindow;
namespace Ui {
class SendFile;
}
class QFile;
class QTcpServer;
class QTcpSocket;
class SendFile : public QDialog
{
    Q_OBJECT

public:
    explicit SendFile(QWidget *parent = nullptr);
    ~SendFile();
    void closeEvent(QCloseEvent *event) override;
//setter&getter
public:
    void setChatWindow(ChatWindow* chatWindow);
//slots
private slots:
    void on_selectFileButton_clicked();
    void on_cancelButton_clicked();
    void on_sendFileButton_clicked();
    void newConnection();
    void disConnected();
    void sendData();
private:
    Ui::SendFile *ui;
    ChatWindow* chatWindow;
    QTcpServer* tcpServer;
    QTcpSocket* tcpSocket;
    QFile* localFile;
    QString filePath;
    QString fileName;
    qint64 fileSize;
    QElapsedTimer timer;
    qint64 haveSendSize;
    qint64 notCountSize;
    qint16 port;
};

#endif // SENDFILE_H
