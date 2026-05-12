#ifndef MOCKNETWORKCLIENT_H
#define MOCKNETWORKCLIENT_H

#include "INetworkClient.h"
#include <functional>

class MockNetworkClient : public INetworkClient {
private:
    std::function<void(const std::string&)> messageHandler;

public:
    bool connectToServer(const std::string& username,
                         const std::string& host,
                         unsigned short port) override;
    void sendMessage(const std::string& message) override;
    void setMessageHandler(std::function<void(const std::string&)> handler) override;
};

#endif
