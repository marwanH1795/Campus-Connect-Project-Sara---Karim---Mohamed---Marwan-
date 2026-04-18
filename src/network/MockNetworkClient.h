#ifndef MOCKNETWORKCLIENT_H
#define MOCKNETWORKCLIENT_H

#include "INetworkClient.h"
#include <functional>

class MockNetworkClient : public INetworkClient {
private:
    std::function<void(const std::string&)> messageHandler;

public:
    void connectToServer(const std::string& username) override;
    void sendMessage(const std::string& message) override;
    void setMessageHandler(std::function<void(const std::string&)> handler) override;
};

#endif
