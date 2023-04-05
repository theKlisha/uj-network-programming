/*
    <request-message> :== <request-list> <endl>

    <response-message> :== <response-list> <endl>

    <request-list> ::=
        | <expression> <endl>
        | <expression> <endl> <request-list>

    <response-list> ::=
        | <result> <endl>
        | <result> <endl> <response-list>

    <expression> ::=
        | <number>
        | <expression> <operator> <expression>

    <result> ::=
        | <number>
        | "ERROR"

    <number> ::=
        | <digit>
        | <digit> <number>

    <digit> ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

    <operator> ::= "+" | "-"

    <endl> ::= "\r\n"
*/

#include <fstream>
#include <iostream>
#include <istream>
#include <optional>
#include <stdint.h>
#include <streambuf>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <utility>
#include <variant>
#include <vector>

namespace arpa {
#include <arpa/inet.h>
}; // namespace arpa

namespace sys {
#include <sys/socket.h>
#include <sys/types.h>
}; // namespace sys

class Server;

class Server {
    using handler_t = std::function<void(int)>;

    class ServerBuilder {
      private:
        std::string m_addr_str = "0.0.0.0";
        uint16_t m_port = 0;
        handler_t m_handler;

      public:
        ServerBuilder() {
        }

        ServerBuilder &handler(handler_t handler) {
            return *this;
        }

        ServerBuilder &addr(std::string addr_str) {
            m_addr_str = addr_str;
            return *this;
        }

        ServerBuilder &port(uint16_t port) {
            m_port = port;
            return *this;
        }

        ServerBuilder &hander(handler_t handler) {
            m_handler = handler;
            return *this;
        }

        Server build() {
            auto server = Server(m_addr_str, m_port, m_handler);
            return server;
        }
    };

  private:
    std::string m_addr_str;
    uint16_t m_port;
    handler_t m_handler;
    int m_sock_fd;

    void m_assert_ok(bool condition, std::string message) {
        if (!condition) {
            throw std::runtime_error(
                message + ": " + std::string(strerror(errno))
            );
        }
    }

    static void m_accept(int client_sock_fd) {
        close(client_sock_fd);
    }

    Server(std::string addr_str, uint16_t port, handler_t handler)
        : m_addr_str(addr_str), m_port(port), m_handler(handler) {
    }

  public:
    static ServerBuilder builder() {
        return ServerBuilder();
    }

    void listen() {
        uint32_t addr_raw = 0;
        int result = 0;

        result = arpa::inet_pton(AF_INET, m_addr_str.c_str(), &addr_raw);
        m_assert_ok(result != -1, "Failed to parse address");

        m_sock_fd = arpa::socket(AF_INET, SOCK_STREAM, 0);
        m_assert_ok(m_sock_fd != -1, "Failed to create socket");

        arpa::sockaddr_in sock_addr = {
            .sin_len = 0,
            .sin_family = AF_INET,
            .sin_port = htons(m_port),
            .sin_addr = {.s_addr = addr_raw},
        };

        arpa::sockaddr *sock_addr_ptr = (arpa::sockaddr *)&sock_addr;
        result = arpa::bind(m_sock_fd, sock_addr_ptr, sizeof(sock_addr));
        m_assert_ok(m_sock_fd != -1, "Failed to bind socket");

        result = arpa::listen(m_sock_fd, 10);
        m_assert_ok(result != -1, "Failed to listen on socket");

        while (true) {
            int client_sock_fd = arpa::accept(m_sock_fd, NULL, NULL);
            m_assert_ok(client_sock_fd != -1, "Failed to accept connection");
            std::thread(m_handler, client_sock_fd).detach();
        }
    }
};

void protocol_handler(int fd) {
    char greeting[] = "Hello World!\r\n";
    write(fd, greeting, sizeof(greeting));
    close(fd);
}

int main() {
    Server::builder()
        .addr("0.0.0.0")
        .port(2020)
        .hander(protocol_handler)
        .build()
        .listen();
}
