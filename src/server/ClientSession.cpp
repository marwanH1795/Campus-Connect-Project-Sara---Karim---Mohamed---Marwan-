#include "ClientSession.h"
#include "ServerController.h"

#include <boost/asio/write.hpp>

ClientSession::ClientSession(tcp::socket socket,
                             std::shared_ptr<ServerController> controller)
    : socket(std::move(socket)),
      controller(controller)
{
}

void ClientSession::start() {
    controller->registerClient(shared_from_this());
    read();
}

void ClientSession::read() {
    auto self = shared_from_this();

    socket.async_read_some(
        boost::asio::buffer(readBuffer),
        [this, self](boost::system::error_code ec, std::size_t bytesRead) {
            if (!ec) {
                pendingText.append(readBuffer.data(), bytesRead);

                std::size_t newlinePosition;

                while ((newlinePosition = pendingText.find('\n')) != std::string::npos) {
                    std::string line = pendingText.substr(0, newlinePosition);
                    pendingText.erase(0, newlinePosition + 1);

                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }

                    if (!line.empty()) {
                        controller->handleMessage(line, self);
                    }
                }

                read();
            } else {
                controller->removeClient(self);

                boost::system::error_code ignoredError;
                socket.close(ignoredError);
            }
        }
    );
}

void ClientSession::send(const std::string& msg) {
    auto self = shared_from_this();
    auto data = std::make_shared<std::string>(msg + "\n");

    boost::asio::async_write(
        socket,
        boost::asio::buffer(*data),
        [this, self, data](boost::system::error_code ec, std::size_t) {
            if (ec) {
                boost::system::error_code ignoredError;
                socket.close(ignoredError);
            }
        }
    );
}

void ClientSession::setUsername(const std::string& newUsername) {
    username = newUsername;
}

std::string ClientSession::getUsername() const {
    return username;
}
