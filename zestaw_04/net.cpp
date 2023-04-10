#include "net.hpp"

namespace net {
    TcpServer::TcpServer(
        std::string addr_str, uint16_t port, Handler const &handler
    )
        : m_addr_str(addr_str), m_port(port), m_handler(handler) {
    }

    void TcpServer::m_assert_ok(bool condition, std::string message) {
        if (!condition) {
            throw std::runtime_error(
                message + ": " + std::string(strerror(errno))
            );
        }
    }

    void TcpServer::listen() {
        uint32_t addr_raw = 0;
        int result = 0;

        result = inet_pton(AF_INET, m_addr_str.c_str(), &addr_raw);
        m_assert_ok(result != -1, "failed to parse address");

        m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        m_assert_ok(m_sock_fd != -1, "failed to create socket");

        sockaddr_in sock_addr = {
            .sin_len = 0,
            .sin_family = AF_INET,
            .sin_port = htons(m_port),
            .sin_addr = {.s_addr = addr_raw},
        };

        sockaddr *sock_addr_ptr = (sockaddr *)&sock_addr;
        result = bind(m_sock_fd, sock_addr_ptr, sizeof(sock_addr));
        m_assert_ok(m_sock_fd != -1, "failed to bind socket");

        result = ::listen(m_sock_fd, 10);
        m_assert_ok(result != -1, "failed to listen on socket");

        while (true) {
            int client_sock_fd = accept(m_sock_fd, NULL, NULL);
            m_assert_ok(client_sock_fd != -1, "failed to accept connection");
            std::thread(m_accept, this, client_sock_fd).detach();
        }
    }

    Context::Context(int sock_fd) : sock_fd(sock_fd) {
    }

    Context::Context(const Context &&other) : sock_fd(other.sock_fd) {
        this->sock_fd = -1;
    }

    Context::~Context() {
        close(sock_fd);
    }

    int Context::raw() {
        return sock_fd;
    }

    StremReader Context::stream_reader() {
        return StremReader(sock_fd);
    }

    int Context::write_string(std::string_view str) {
        return write(sock_fd, str.data(), str.length());
    }
} // namespace net
