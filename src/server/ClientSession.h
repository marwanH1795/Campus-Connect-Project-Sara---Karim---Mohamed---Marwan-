#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

using boost::asio::ip::tcp;

class ServerController;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
private:
    tcp::socket socket;
    std::shared_ptr<ServerController> controller;

    std::array<char, 4> headerBuffer;
    std::vector<char> payloadBuffer;

    std::string username;

    void readHeader();
    void readPayload(std::uint32_t payloadSize);

    static std::uint32_t decodeBigEndianLength(const std::array<char, 4>& buffer);
    static std::array<char, 4> encodeBigEndianLength(std::uint32_t value);

public:
    ClientSession(tcp::socket socket,
                  std::shared_ptr<ServerController> controller);

    void start();

    void send(const std::string& msg);
    void sendBinary(const char* data, std::size_t size);

    void setUsername(const std::string& newUsername);
    std::string getUsername() const;
};

#endif
