#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class ServerController;

class TcpServer {
private:
    tcp::acceptor acceptor;
    std::shared_ptr<ServerController> controller;

    void acceptNext();

public:
    TcpServer(boost::asio::io_context& ioContext, unsigned short port);
};

#endif
