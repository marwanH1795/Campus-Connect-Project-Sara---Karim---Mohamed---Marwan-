#include "ClientSession.h"
#include "ServerController.h"

#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>

ClientSession::ClientSession(tcp::socket socket,
                             std::shared_ptr<ServerController> controller)
    : socket(std::move(socket)),
      controller(controller)
{
}

void ClientSession::start() {
    controller->registerClient(shared_from_this());
    readHeader();
}

std::uint32_t ClientSession::decodeBigEndianLength(const std::array<char, 4>& buffer) {
    return (static_cast<unsigned char>(buffer[0]) << 24) |
           (static_cast<unsigned char>(buffer[1]) << 16) |
           (static_cast<unsigned char>(buffer[2]) << 8) |
           (static_cast<unsigned char>(buffer[3]));
}

std::array<char, 4> ClientSession::encodeBigEndianLength(std::uint32_t value) {
    return {
        static_cast<char>((value >> 24) & 0xFF),
        static_cast<char>((value >> 16) & 0xFF),
        static_cast<char>((value >> 8) & 0xFF),
        static_cast<char>(value & 0xFF)
    };
}

void ClientSession::readHeader() {
    auto self = shared_from_this();

    boost::asio::async_read(
        socket,
        boost::asio::buffer(headerBuffer),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                controller->removeClient(self);

                boost::system::error_code ignoredError;
                socket.close(ignoredError);
                return;
            }

            std::uint32_t payloadSize = decodeBigEndianLength(headerBuffer);

            if (payloadSize == 0) {
                readHeader();
                return;
            }

            readPayload(payloadSize);
        }
    );
}

void ClientSession::readPayload(std::uint32_t payloadSize) {
    auto self = shared_from_this();

    payloadBuffer.resize(payloadSize);

    boost::asio::async_read(
        socket,
        boost::asio::buffer(payloadBuffer),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec) {
                controller->removeClient(self);

                boost::system::error_code ignoredError;
                socket.close(ignoredError);
                return;
            }

            if (payloadBuffer.empty()) {
                readHeader();
                return;
            }

            char frameType = payloadBuffer[0];

            if (frameType == 0) {
                std::string payload;

                if (payloadBuffer.size() > 1) {
                    payload.assign(payloadBuffer.begin() + 1, payloadBuffer.end());
                }

                controller->handleMessage(payload, self);
            } else if (frameType == 1) {
                if (payloadBuffer.size() > 1) {
                    controller->handleBinary(
                        payloadBuffer.data() + 1,
                        payloadBuffer.size() - 1,
                        self
                    );
                }
            }

            readHeader();
        }
    );
}

void ClientSession::send(const std::string& msg) {
    auto self = shared_from_this();

    std::uint32_t payloadSize = static_cast<std::uint32_t>(msg.size() + 1);

    auto frame = std::make_shared<std::vector<char>>();
    frame->reserve(4 + payloadSize);

    std::array<char, 4> header = encodeBigEndianLength(payloadSize);

    frame->insert(frame->end(), header.begin(), header.end());
    frame->push_back(0);
    frame->insert(frame->end(), msg.begin(), msg.end());

    boost::asio::async_write(
        socket,
        boost::asio::buffer(*frame),
        [this, self, frame](boost::system::error_code ec, std::size_t) {
            if (ec) {
                controller->removeClient(self);

                boost::system::error_code ignoredError;
                socket.close(ignoredError);
            }
        }
    );
}

void ClientSession::sendBinary(const char* data, std::size_t size) {
    auto self = shared_from_this();

    std::uint32_t payloadSize = static_cast<std::uint32_t>(size + 1);

    auto frame = std::make_shared<std::vector<char>>();
    frame->reserve(4 + payloadSize);

    std::array<char, 4> header = encodeBigEndianLength(payloadSize);

    frame->insert(frame->end(), header.begin(), header.end());
    frame->push_back(1);
    frame->insert(frame->end(), data, data + size);

    boost::asio::async_write(
        socket,
        boost::asio::buffer(*frame),
        [this, self, frame](boost::system::error_code ec, std::size_t) {
            if (ec) {
                controller->removeClient(self);

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
