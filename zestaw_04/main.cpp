#include "TcpServer.hpp"
#include <string>

class ConnectionHandler : public TcpServerHandler {
  public:
    void handle(TcpServerContext context) const override {
        std::string error = "ERROR\r\n";

        auto reader = LineReader(context.raw());
        auto line = reader.read_line();

        std::cout << line << std::endl;

        write(context.raw(), error.data(), error.length());
    }
};

int main() {
    auto handler = ConnectionHandler();
    auto server = TcpServer("0.0.0.0", 2020, handler);

    server.listen();
}
