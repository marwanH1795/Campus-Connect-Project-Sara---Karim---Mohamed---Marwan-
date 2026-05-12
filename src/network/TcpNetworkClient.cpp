#include "TcpNetworkClient.h"

#include <QDataStream>
#include <QDebug>
#include <QString>

TcpNetworkClient::TcpNetworkClient() {
    socket = new QTcpSocket();

    QObject::connect(socket, &QTcpSocket::readyRead, [this]() {
        buffer.append(socket->readAll());
        processBuffer();
    });

    QObject::connect(socket, &QTcpSocket::connected, []() {
        qDebug() << "CONNECTED TO SERVER";
    });

    QObject::connect(socket, &QTcpSocket::disconnected, []() {
        qDebug() << "DISCONNECTED FROM SERVER";
    });
}

TcpNetworkClient::~TcpNetworkClient() {
    socket->close();
    delete socket;
}

void TcpNetworkClient::setMessageHandler(std::function<void(const std::string&)> handler) {
    messageHandler = handler;
}

void TcpNetworkClient::setBinaryHandler(std::function<void(const QByteArray&)> handler) {
    binaryHandler = handler;
}

bool TcpNetworkClient::connectToServer(const std::string& username,
                                       const std::string& host,
                                       unsigned short port) {
    if (socket->state() != QAbstractSocket::UnconnectedState) {
        socket->abort();
    }

    buffer.clear();
    socket->connectToHost(QString::fromStdString(host), port);

    if (!socket->waitForConnected(3000)) {
        qDebug() << "Failed to connect to server:"
                 << QString::fromStdString(host)
                 << port
                 << socket->errorString();
        return false;
    }

    QString connectMessage =
        QString("{\"type\":\"connect\",\"sender\":\"%1\",\"target\":\"\",\"groupId\":\"\",\"content\":\"\",\"timestamp\":\"\"}")
            .arg(QString::fromStdString(username));

    sendMessage(connectMessage.toStdString());
    return true;
}

void TcpNetworkClient::sendMessage(const std::string& message) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Cannot send message: socket is not connected";
        return;
    }

    QByteArray payload = QByteArray::fromStdString(message);

    QByteArray frame;
    QDataStream stream(&frame, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << quint32(payload.size() + 1);
    frame.append(char(0));
    frame.append(payload);

    socket->write(frame);
    socket->flush();

    qDebug() << "CLIENT SENT TEXT FRAME, size:" << payload.size();
}

void TcpNetworkClient::sendBinary(const QByteArray& data) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Cannot send binary: socket is not connected";
        return;
    }

    QByteArray frame;
    QDataStream stream(&frame, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    stream << quint32(data.size() + 1);
    frame.append(char(1));
    frame.append(data);

    socket->write(frame);
    socket->flush();

    qDebug() << "CLIENT SENT BINARY FRAME, size:" << data.size();
}

void TcpNetworkClient::processBuffer() {
    while (true) {
        if (buffer.size() < 4) {
            return;
        }

        QDataStream stream(buffer);
        stream.setByteOrder(QDataStream::BigEndian);

        quint32 frameSize;
        stream >> frameSize;

        if (frameSize == 0) {
            buffer.remove(0, 4);
            continue;
        }

        if (buffer.size() < int(4 + frameSize)) {
            return;
        }

        QByteArray frame = buffer.mid(4, frameSize);
        buffer.remove(0, 4 + frameSize);

        if (frame.isEmpty()) {
            continue;
        }

        char frameType = frame[0];
        QByteArray payload = frame.mid(1);

        if (frameType == 0) {
            qDebug() << "CLIENT RECEIVED TEXT FRAME, size:" << payload.size();

            if (messageHandler) {
                messageHandler(payload.toStdString());
            }
        }
        else if (frameType == 1) {
            qDebug() << "CLIENT RECEIVED BINARY FRAME, size:" << payload.size();

            if (binaryHandler) {
                binaryHandler(payload);
            }
        }
        else {
            qDebug() << "CLIENT RECEIVED UNKNOWN FRAME TYPE:" << int(frameType);
        }
    }
}
