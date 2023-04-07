#include "TcpServer.hpp"

TcpServer::TcpServer(
    std::string addr_str, uint16_t port, TcpServerHandler const &handler
)
    : m_addr_str(addr_str), m_port(port), m_handler(handler) {
}

void TcpServer::m_assert_ok(bool condition, std::string message) {
    if (!condition) {
        throw std::runtime_error(message + ": " + std::string(strerror(errno)));
    }
}

void TcpServer::listen() {
    uint32_t addr_raw = 0;
    int result = 0;

    result = inet_pton(AF_INET, m_addr_str.c_str(), &addr_raw);
    m_assert_ok(result != -1, "Failed to parse address");

    m_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    m_assert_ok(m_sock_fd != -1, "Failed to create socket");

    sockaddr_in sock_addr = {
        .sin_len = 0,
        .sin_family = AF_INET,
        .sin_port = htons(m_port),
        .sin_addr = {.s_addr = addr_raw},
    };

    sockaddr *sock_addr_ptr = (sockaddr *)&sock_addr;
    result = bind(m_sock_fd, sock_addr_ptr, sizeof(sock_addr));
    m_assert_ok(m_sock_fd != -1, "Failed to bind socket");

    result = ::listen(m_sock_fd, 10);
    m_assert_ok(result != -1, "Failed to listen on socket");

    while (true) {
        int client_sock_fd = accept(m_sock_fd, NULL, NULL);
        m_assert_ok(client_sock_fd != -1, "Failed to accept connection");
        std::thread(m_accept, this, client_sock_fd).detach();
    }
}

LineReader::LineReader(int fd)
    : m_fd(fd), m_buffer(std::vector<uint8_t>(m_BUFFER_CHUNK_SIZE)),
      m_position(0) {
}

std::string_view LineReader::read_line() {
    while (true) {
        // make shure we have enough space in the buffer
        m_buffer.reserve(m_position + m_BUFFER_CHUNK_SIZE);

        // read data directly into the buffer
        int bytes_read = read(
            m_fd, m_buffer.data() + m_position, m_buffer.capacity() - m_position
        );

        if (bytes_read == 0) {
            // end of file
            return std::string_view();
        } else if (bytes_read == -1) {
            // error
            throw std::runtime_error(
                "Failed to read from socket: " + std::string(strerror(errno))
            );
        }

        // set end of buffer to the end of the data
        m_buffer.resize(m_position + bytes_read);

        // search for a line break
        for (int i = m_position; i < m_buffer.size(); i++) {
            if (m_buffer[i] == '\r' && m_buffer[i + 1] == '\n') {
                std::string_view line(
                    (char *)m_buffer.data() + m_position, i - m_position
                );
                m_position = i + 2;
                return line;
            }
        }
    }
}

TcpServerContext::TcpServerContext(int sock_fd) : sock_fd(sock_fd) {
}

TcpServerContext::TcpServerContext(const TcpServerContext &&other)
    : sock_fd(other.sock_fd) {
    this->sock_fd = -1;
}

TcpServerContext::~TcpServerContext() {
    close(sock_fd);
}

int TcpServerContext::raw() {
    return sock_fd;
}
