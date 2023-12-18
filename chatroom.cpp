//
// Created by WuShF on 2023/12/17.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ChatRoom.h" resolved

#include <QImageReader>
#include "chatroom.h"
#include "ui_ChatRoom.h"
#include "QDateTime"
#include "QProcess"
#include "QKeyEvent"
#include "QMimeData"
#include "QImage"
#include "tools/FileConvert.h"
#include "QClipboard"


ChatRoom::ChatRoom(QWidget *parent) : ui(new Ui::ChatRoom) {
    ui->setupUi(this);
    castType = Broadcast;
    ui->messageTextEdit->installEventFilter(this);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &ChatRoom::sendMessageFromTextEdit);
    connect(ui->sendFileButton, &QPushButton::clicked, this, &ChatRoom::sendFile);
    connect(ui->switchCastTypeButton, &QPushButton::clicked, this, &ChatRoom::switchCastType);
    sendMessage(UserJoin);
}

ChatRoom::~ChatRoom() {
    sendMessage(UserLeft);
    delete ui;
}

void ChatRoom::readMessage() {
    QByteArray data;
    data.resize(udpSocket->pendingDatagramSize());
    udpSocket->readDatagram(data.data(), data.size());
    QDataStream in(&data, QIODevice::ReadOnly);
    MessageType messageType;
    CastType castType;
    QString serverUserName;
    QString serverHostName;
    QString serverIPAddress;
    in >> messageType >> castType >> serverUserName >> serverHostName >> serverIPAddress;
    if (!isOnline(serverIPAddress)) {
        sendMessage(UserJoin);
        setOnline(serverUserName, serverHostName, serverIPAddress);
    }
    switch (messageType) {
        case MessageSend: {
            QString message;
            in >> message;
            qDebug() << message;
            if (castType == Broadcast) {
                //broadcast
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(
                        tr("%1  %2").arg(serverUserName, QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::black);
                ui->messageTextBrowser->append(message);
            } else {
                //unicast
                if (serverIPAddress == localIpAddress) {
                    return;
                }
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(
                        tr("%1  %2").arg(serverUserName, QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::gray);
                ui->messageTextBrowser->append(tr("私发给你一条消息"));
                ui->messageTextBrowser->setTextColor(Qt::black);
                ui->messageTextBrowser->append(message);
            }
            break;
        }
        case UserJoin: {
            return;
        }
        case UserLeft: {
            if (serverIPAddress == localIpAddress) {
                sendMessage(UserJoin);
            }
            if (!setOffine(serverIPAddress)) {
                return;
            }
            ui->messageTextBrowser->setTextColor(Qt::blue);
            ui->messageTextBrowser->append(tr("%1  %2").arg(serverUserName, QDateTime::currentDateTime().toString()));
            ui->messageTextBrowser->setTextColor(Qt::gray);
            ui->messageTextBrowser->append(tr("下线了"));
            break;
        }
        case FileSend: {
            qint16 port;
            QString fileName;
            qint64 fileSize;
            if (castType == Broadcast) {
                in >> port >> fileName >> fileSize;
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(
                        tr("%1  %2").arg(serverUserName, QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::gray);
                ui->messageTextBrowser->append(tr("广播了一个文件： ") + fileName);
            } else {
                QString tmpIpAddress;
                in >> tmpIpAddress >> port >> fileName >> fileSize;
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(
                        tr("%1  %2").arg(serverUserName, QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::gray);
                ui->messageTextBrowser->append(tr("私发给你一个文件： ") + fileName);
            }
            QProcess process;
            process.setProgram("FileReceiver.exe");
            QStringList arguments;
            arguments.append(serverIPAddress);
            arguments.append(QString::number(port));
            arguments.append(fileName);
            arguments.append(QString::number(fileSize));
            process.setArguments(arguments);
            process.startDetached();
            break;
        }
    }
}

bool ChatRoom::isOnline(QString ipAddress) {
    auto users = ui->userTableWidget->findItems(ipAddress, Qt::MatchExactly);
    if (users.isEmpty()) {
        return false;
    }
    return true;
}

void ChatRoom::setOnline(const QString userName, const QString hostName, const QString iPAddress) {
    QTableWidgetItem *user = new QTableWidgetItem(userName);
    QTableWidgetItem *host = new QTableWidgetItem(hostName);
    QTableWidgetItem *ip = new QTableWidgetItem(iPAddress);
    ui->userTableWidget->insertRow(0);
    ui->userTableWidget->setItem(0, 0, user);
    ui->userTableWidget->setItem(0, 1, host);
    ui->userTableWidget->setItem(0, 2, ip);
    ui->messageTextBrowser->setTextColor(Qt::blue);
    ui->messageTextBrowser->append(tr("%1  %2").arg(userName, QDateTime::currentDateTime().toString()));
    ui->messageTextBrowser->setTextColor(Qt::gray);
    ui->messageTextBrowser->append(tr("加入房间"));
}

void ChatRoom::sendMessageFromTextEdit() {
    if (ui->messageTextEdit->toPlainText().isEmpty()) {
        ui->statusbar->showMessage(tr("不要发送空消息"));
        return;
    }
    QString message = ui->messageTextEdit->toHtml();
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << message;
    sendMessage(MessageSend, NotDefine, data);
    ui->messageTextEdit->clear();
}

bool ChatRoom::setOffine(const QString iPAddress) {
    auto findItems = ui->userTableWidget->findItems(iPAddress, Qt::MatchExactly);
    if (findItems.isEmpty()) {
        return false;
    }
    int rowNum = findItems.first()->row();
    ui->userTableWidget->removeRow(rowNum);
    return true;
}

bool ChatRoom::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            if (keyEvent->modifiers() & Qt::ControlModifier) {
                ui->messageTextEdit->insertPlainText("\n");
            } else {
                QTextCursor cursor = ui->messageTextEdit->textCursor();
                int cursorPos = cursor.position();
                if (cursorPos >= 0 && cursorPos <= ui->messageTextEdit->toPlainText().length()) {
                    sendMessageFromTextEdit();
                }
            }
            return true;
        } else if (keyEvent->matches(QKeySequence::Paste)) {
            QClipboard *clipboard = QApplication::clipboard();
            const QMimeData *mimeData = clipboard->mimeData();
            if (mimeData->hasImage()) {
                QPixmap pixmap = clipboard->pixmap();
                QImage image = pixmap.toImage();
                image = image.scaledToHeight(100);
                QString base64Image = FileConvert::imageToBase64(image);
                ui->messageTextEdit->textCursor().insertHtml("<img src='" + base64Image + "'/>");
                return true;
            } else if (mimeData->hasUrls()) {
                QList<QUrl> urls = mimeData->urls();
                int imageFlag = 0;
                        foreach(QUrl url, urls) {
                        QString localPath = url.toLocalFile();
                        QImageReader reader(localPath);
                        QImage image = reader.read();
#ifdef DEBUG
                        ui->messageTextBrowser->append(tr("pastePath: ") + localPath);
                        if (image.isNull()) {
                            ui->messageTextBrowser->append(tr("errorString: ") + reader.errorString());
                        }
#endif
                        if (!image.isNull()) {
                            imageFlag = 1;
                            image = image.scaledToHeight(100);
                            QString base64Image = FileConvert::imageToBase64(image);
                            ui->messageTextEdit->textCursor().insertHtml("<img src='" + base64Image + "'/>");
                        }
                    }
                if (imageFlag) {
                    return true;
                }
            }
        }
    }
    return false;
}

void ChatRoom::switchCastType() {
    if (castType == Broadcast) {
        castType = Unicast;
        ui->switchCastTypeButton->setText(tr("切换消息范围：当前一对一单播"));
    } else {
        castType = Broadcast;
        ui->switchCastTypeButton->setText(tr("切换消息范围：当前局域网多播"));
    }
}

QList<QTableWidgetItem *> ChatRoom::getSelectedUserInfo() {
#ifdef DEBUG
    auto selectedItems = ui->userTableWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        ui->messageTextBrowser->append(tr("selectedItemsIsEmpty"));
    } else {
                foreach(QTableWidgetItem *item, selectedItems) {
                ui->messageTextBrowser->append(item->text());
            }
    }
#endif
    return ui->userTableWidget->selectedItems();
}

void ChatRoom::sendMessage(UdpServer::MessageType messageType, UdpServer::CastType castType, QByteArray data) {
    QByteArray messageData;
    QDataStream out(&messageData, QIODevice::WriteOnly);
    if (castType == NotDefine) {
        castType = this->castType;
    }
    out << messageType << castType << localUserName << localHostName << localIpAddress;
    switch (messageType) {
        case MessageSend: {
            messageData += data;
            if (castType == Broadcast) {
                udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress::Broadcast, port);
                break;
            } else {
                auto selectedUser = getSelectedUserInfo();
                if (selectedUser.isEmpty()) {
                    ui->statusbar->showMessage(tr("先选择要私发的用户"));
                    return;
                }
                QString serverUserName = selectedUser[0]->text();
                QString serverIpAddress = selectedUser[2]->text();
                QDataStream in(&data, QIODevice::ReadOnly);
                QString message;
                in >> message;
                ui->messageTextBrowser->setTextColor(Qt::blue);
                ui->messageTextBrowser->append(
                        tr("%1  %2").arg(localUserName, QDateTime::currentDateTime().toString()));
                ui->messageTextBrowser->setTextColor(Qt::gray);
                ui->messageTextBrowser->append(tr("你私发给%1一条消息").arg(serverUserName));
                ui->messageTextBrowser->setTextColor(Qt::black);
                ui->messageTextBrowser->append(message);
                udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress(serverIpAddress), port);
                break;
            }
        }
        case FileSend: {
            messageData += data;
            udpSocket->writeDatagram(messageData, messageData.size(), QHostAddress::Broadcast, port);
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

void ChatRoom::sendFile() {
    QProcess process;
    QList<QString> arguments;
    if (castType == Unicast) {
        auto selectedUser = getSelectedUserInfo();
        if (selectedUser.isEmpty()) {
            ui->statusbar->showMessage(tr("先选择要私发的用户"));
            return;
        }
        QProcess process;
        process.setProgram("FileSender.exe");

        QString ipAddress = selectedUser[2]->text();
        arguments.append(tr("Unicast"));
        arguments.append(ipAddress);
    } else {
        arguments.append(tr("Broadcast"));
    }
    process.setArguments(arguments);
    process.setProgram("FileSender.exe");
    process.startDetached();
}
