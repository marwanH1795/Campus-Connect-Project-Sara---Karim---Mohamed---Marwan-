#include "TcpServer.h"
#include "ClientSession.h"

#include <iostream>
#include <memory>

TcpServer::TcpServer(boost::asio::io_context& ioContext, unsigned short port)
    : acceptor(ioContext, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "Boost.Asio server running on port " << port << std::endl;
    acceptNext();
}

void TcpServer::acceptNext() {
    acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<ClientSession>(std::move(socket), controller)->start();
        } else {
            std::cout << "Accept error: " << ec.message() << std::endl;
        }

        acceptNext();
    });
}
