#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <array>
#include <deque>
#include <memory>
#include <string>

class ServerController;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
public:
    using tcp = boost::asio::ip::tcp;

    ClientSession(tcp::socket socket, ServerController& controller);

    void start();
    void send(const std::string& jsonMessage);

private:
    void readData();
    void processIncomingData(const std::string& data);
    void writeNext();
    void close();

    tcp::socket socket;
    ServerController& controller;

    std::array<char, 4096> readBuffer;
    std::string incomingBuffer;
    std::deque<std::string> writeQueue;
};
