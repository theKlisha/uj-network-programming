#pragma once
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

class TcpServer;
class TcpServerContext;
class TcpServerHandler;
class TcpServerBuilder;
class LineReader;

class TcpServer {
  private:
    friend class TcpServerBuilder;
    std::string m_addr_str;
    uint16_t m_port;
    const TcpServerHandler &m_handler;
    int m_sock_fd;
    void m_assert_ok(bool condition, std::string message);
    static void m_accept(TcpServer const *self, int sock_fd);

  public:
    TcpServer(
        std::string addr_str, uint16_t port, TcpServerHandler const &handler
    );

    void listen();
};

class LineReader {
  private:
    const int m_BUFFER_CHUNK_SIZE = 1024 * 10;
    int m_fd;
    std::vector<uint8_t> m_buffer;
    int m_position;

  public:
    LineReader(int fd);
    std::string_view read_line();
};

class TcpServerContext {
    int sock_fd;

  public:
    TcpServerContext(int sock_fd);
    TcpServerContext(const TcpServerContext &) = delete;
    TcpServerContext(const TcpServerContext &&);
    ~TcpServerContext();

    int raw();
};

class TcpServerHandler {
  public:
    virtual void handle(TcpServerContext context) const = 0;
};

inline void TcpServer::m_accept(TcpServer const *self, int sock_fd) {
    self->m_handler.handle(TcpServerContext(sock_fd));
}
