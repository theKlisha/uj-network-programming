#include "net.hpp"
#include "protocol.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

class Handler : public net::Handler {
  private:
  public:
    void handle(net::Context context) const override {
        auto lexer = protocol::Lexer(context.stream_reader());

        while (true) {
            protocol::ParsedLine line;

            try {
                line = protocol::parse_line(lexer);
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
                context.write_string("ERROR\r\n");
                return;
            }

            if (std::holds_alternative<protocol::ParsedLineValue>(line)) {
                auto value = std::get<protocol::ParsedLineValue>(line).value;
                context.write_string(std::to_string(value) + "\r\n");
            } else if (std::holds_alternative<protocol::ParsedLineError>(line)) {
                auto e = std::get<protocol::ParsedLineError>(line).error;
                std::cout << e.what() << std::endl;
                context.write_string("ERROR\r\n");
            } else if (std::holds_alternative<protocol::ParsedLineEmpty>(line)) {
                context.write_string("\r\n");
                return;
            }
        }
    }
};

int main() {
    auto handler = Handler();
    auto server = net::TcpServer("0.0.0.0", 2020, handler);

    server.listen();
}
