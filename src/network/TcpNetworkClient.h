#ifndef TCPNETWORKCLIENT_H
#define TCPNETWORKCLIENT_H

#include "INetworkClient.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QStringList>

class TcpNetworkClient : public INetworkClient {
private:
    QTcpSocket* socket;
    std::function<void(const std::string&)> messageHandler;
    QByteArray buffer;
    QStringList pendingMessages;

    void processBuffer();

public:
    TcpNetworkClient();
    ~TcpNetworkClient();

    void connectToServer(const std::string& username) override;
    void sendMessage(const std::string& message) override;
    void setMessageHandler(std::function<void(const std::string&)> handler) override;
};

#endif
