#pragma once
#include <arpa/inet.h>
#include <array>
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
#include <cstring>

namespace net {
    class TcpServer;
    class Context;
    class Handler;
    class StremReader;

    class TcpServer {
      private:
        friend class TcpServerBuilder;
        std::string m_addr_str;
        uint16_t m_port;
        const Handler &m_handler;
        int m_sock_fd;
        void m_assert_ok(bool condition, std::string message);
        static void m_accept(TcpServer const *self, int sock_fd);

      public:
        TcpServer(std::string addr_str, uint16_t port, Handler const &handler);

        void listen();
    };

    class Context {
        int sock_fd;

      public:
        Context(int sock_fd);
        Context(const Context &) = delete;
        Context(const Context &&);
        ~Context();

        int raw();
        StremReader stream_reader();
        int write_string(std::string_view str);
    };

    class Handler {
      public:
        virtual void handle(Context context) const = 0;
    };

    inline void TcpServer::m_accept(TcpServer const *self, int sock_fd) {
        self->m_handler.handle(Context(sock_fd));
    }

    class StremReader {
      private:
        const int m_fd;
        int m_index = 0;
        int m_size = 0;
        std::array<uint8_t, 1024> m_buffer = std::array<uint8_t, 1024>();

        void m_read_next_chunk() {
            int bytes_red = ::read(m_fd, m_buffer.data(), m_buffer.size());

            if (bytes_red == -1) {
                throw std::runtime_error("failed to read");
            } else if (bytes_red == 0) {
                throw std::runtime_error("connection closed");
            } else {
                m_size = bytes_red;
            }

            m_index = 0;
        }

      public:
        StremReader(int fd)
            : m_fd(fd) {
        }

        uint8_t read() {
            if (m_index >= m_size) {
                m_read_next_chunk();
            }

            return m_buffer[m_index++];
        }

        uint8_t peek() {
            if (m_index >= m_size) {
                m_read_next_chunk();
            }

            return m_buffer[m_index];
        }
    };

} // namespace net
