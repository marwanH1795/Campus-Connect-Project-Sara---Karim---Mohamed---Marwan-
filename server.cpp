#include <boost/asio/io_context.hpp>
#include <cstdio>
#include <exception>

#include "src/server/TcpServer.h"

int main() {
    try {
        boost::asio::io_context ioContext;

        TcpServer server(ioContext, 12345);

        ioContext.run();
    }
    catch (const std::exception& e) {
        std::printf("Server error: %s\n", e.what());
    }

    return 0;
}
