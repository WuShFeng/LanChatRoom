#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>
#include<QElapsedTimer>
namespace Ui {
class ReceiveFile;
}
class QTcpSocket;
class QFile;
class ReceiveFile : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFile(QWidget *parent = nullptr);
    ~ReceiveFile();
    void showEvent(QShowEvent* event)override;
    void closeEvent(QCloseEvent *event) override;
//setter&getter
public:
    void setFileName(QString fileName);
    void setFileSize(qint64 fileSize);
    void setPort(qint16 port);
    void setIpAddress(const QString &newIpAddress);
//slots
private slots:
    void on_selectFilePathButton_clicked();
    void getData();
    void connected();
    void on_receiveFileButton_clicked();
    void disconnected();
    void on_cancelButton_clicked();

private:
    Ui::ReceiveFile *ui;
    QTcpSocket* tcpSocket;
    QFile* localFile;
    QString filePath;
    QString fileName;
    QString ipAddress;
    qint64 fileSize;
    qint16 port;
    QElapsedTimer timer;
    qint64 haveSendSize;
    qint64 notCountSize;
};

#endif // RECEIVEFILE_H
