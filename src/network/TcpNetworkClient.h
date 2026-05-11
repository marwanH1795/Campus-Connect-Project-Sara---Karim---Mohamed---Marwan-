#ifndef TCPNETWORKCLIENT_H
#define TCPNETWORKCLIENT_H

#include "INetworkClient.h"

#include <QTcpSocket>
#include <QByteArray>
#include <functional>

class TcpNetworkClient : public INetworkClient {
private:
    QTcpSocket* socket;
    std::function<void(const std::string&)> messageHandler;
    std::function<void(const QByteArray&)> binaryHandler;

    QByteArray buffer;

    void processBuffer();

public:
    TcpNetworkClient();
    ~TcpNetworkClient();

    void connectToServer(const std::string& username,
                         const std::string& host,
                         unsigned short port) override;

    void sendMessage(const std::string& message) override;
    void sendBinary(const QByteArray& data) override;

    void setMessageHandler(std::function<void(const std::string&)> handler) override;
    void setBinaryHandler(std::function<void(const QByteArray&)> handler) override;
};

#endif
