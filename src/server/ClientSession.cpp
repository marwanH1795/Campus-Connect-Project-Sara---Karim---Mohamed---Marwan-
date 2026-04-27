#include "ClientSession.h"
#include "ServerController.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/write.hpp>

#include <iostream>

ClientSession::ClientSession(tcp::socket socket, ServerController& controller)
    : socket(std::move(socket)), controller(controller) {}

void ClientSession::start() {
    controller.onClientConnected(shared_from_this());
    readData();
}

void ClientSession::send(const std::string& jsonMessage) {
    auto self = shared_from_this();

    boost::asio::post(socket.get_executor(), [this, self, jsonMessage]() {
        bool alreadyWriting = !writeQueue.empty();

        std::string line = jsonMessage;
        line += "\n";
        writeQueue.push_back(line);

        if (!alreadyWriting) {
            writeNext();
        }
    });
}

void ClientSession::readData() {
    auto self = shared_from_this();

    socket.async_read_some(
        boost::asio::buffer(readBuffer),
        [this, self](boost::system::error_code ec, std::size_t bytesRead) {
            if (ec) {
                close();
                return;
            }

            std::string data(readBuffer.data(), bytesRead);
            processIncomingData(data);

            readData();
        }
    );
}

void ClientSession::processIncomingData(const std::string& data) {
    incomingBuffer += data;

    while (true) {
        std::size_t newlinePos = incomingBuffer.find('\n');

        if (newlinePos == std::string::npos) {
            break;
        }

        std::string line = incomingBuffer.substr(0, newlinePos);
        incomingBuffer.erase(0, newlinePos + 1);

        if (!line.empty()) {
            controller.onMessageReceived(shared_from_this(), line);
        }
    }
}

void ClientSession::writeNext() {
    auto self = shared_from_this();

    boost::asio::async_write(
        socket,
        boost::asio::buffer(writeQueue.front()),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                close();
                return;
            }

            writeQueue.pop_front();

            if (!writeQueue.empty()) {
                writeNext();
            }
        }
    );
}

void ClientSession::close() {
    boost::system::error_code ignored;
    socket.close(ignored);
    controller.onClientDisconnected(shared_from_this());
}
