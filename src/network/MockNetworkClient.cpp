#include "MockNetworkClient.h"

// set callback
void MockNetworkClient::setMessageHandler(std::function<void(const std::string&)> handler) {
    messageHandler = handler;
}

// simulate connection
void MockNetworkClient::connectToServer(const std::string& username) {
    if (messageHandler) {
        std::string response =
            "{"
            "\"type\":\"connect_ack\","
            "\"status\":\"ok\","
            "\"message\":\"Connected successfully\""
            "}";

        messageHandler(response);
    }
}

// simulate sending message
void MockNetworkClient::sendMessage(const std::string& message) {
    if (messageHandler) {
        // echo message back as if server broadcasted it
        messageHandler(message);
    }
}
