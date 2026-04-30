#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <string>

using boost::asio::ip::tcp;

class ServerController;

class ClientSession : public std::enable_shared_from_this<ClientSession> {
private:
    tcp::socket socket;
    std::shared_ptr<ServerController> controller;

    std::array<char, 1024> readBuffer;
    std::string pendingText;
    std::string username;

    void read();

public:
    ClientSession(tcp::socket socket,
                  std::shared_ptr<ServerController> controller);

    void start();
    void send(const std::string& msg);

    void setUsername(const std::string& newUsername);
    std::string getUsername() const;
};

#endif
