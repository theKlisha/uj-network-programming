#include "protocol.hpp"
#include <iostream>
#include <stdexcept>

namespace protocol {
    Lexer::Lexer(net::StremReader reader)
        : m_reader(reader) {
    }

    Token Lexer::next() {
        uint8_t c = m_reader.read();

        if (c == '\r') {
            if (m_reader.peek() == '\n') {
                m_reader.read();
                m_line_begin = m_cursor;
                m_line_number += 1;
                return TokenEndl();
            } else {
                throw lexer_error(*this, "expected \\n after \\r");
            }
        } else if (c == '+') {
            return TokenOperator{Add};
        } else if (c == '-') {
            return TokenOperator{Sub};
        } else if (isdigit(c)) {
            uint32_t value = c - '0';
            while (isdigit(m_reader.peek())) {
                if (value > ((0xFFFFFFFF) - 9) / 10) {
                    throw lexer_error(*this, "number is too big");
                }
                value = value * 10 + (m_reader.read() - '0');
            }
            return TokenNumber{value};
        } else {
            throw lexer_error(*this);
        }
    }

    Token Lexer::next_line() {
        while (true) {
            if (m_reader.read() == '\r' && m_reader.peek() == '\n') {
                m_reader.read();
                return TokenEndl();
            }
        }
    }

    uint8_t Lexer::peek() {
        return m_reader.peek();
    }

    uint8_t Lexer::read() {
        m_cursor += 1;
        return m_reader.read();
    }

    int Lexer::cursor() const {
        return m_cursor;
    }

    int Lexer::line() const {
        return m_line_number;
    }

    int Lexer::column() const {
        return m_cursor - m_line_begin;
    }

    std::runtime_error lexer_error(const Lexer &lexer, std::string message) {
        return std::runtime_error(
            "lexer error: unexpected character at " + std::to_string(lexer.line()) + ":" +
            std::to_string(lexer.column()) + ": " + message
        );
    }

    std::runtime_error parser_error(const Lexer &lexer, std::string message) {
        return std::runtime_error(
            "parser error: unexpected token at " + std::to_string(lexer.line()) + ":" +
            std::to_string(lexer.column()) + ": " + message
        );
    }

    ParsedLine parse_line(Lexer lexer) {
        auto token = lexer.next();
        int32_t sum = 0;

        if (std::holds_alternative<TokenNumber>(token)) {
            sum = std::get<TokenNumber>(token).value;
        } else if (std::holds_alternative<TokenEndl>(token)) {
            return ParsedLineEmpty{};
        } else {
            throw parser_error(lexer, "expected number");
        }

        while (true) {
            auto token = lexer.next();

            if (std::holds_alternative<TokenEndl>(token)) {
                return ParsedLineValue{sum};
            } else if (std::holds_alternative<TokenOperator>(token)) {
                auto op = std::get<TokenOperator>(token).kind;
                token = lexer.next();

                if (std::holds_alternative<TokenNumber>(token)) {
                    auto number = std::get<TokenNumber>(token).value;

                    if (op == Add) {
                        sum += number;
                    } else if (op == Sub) {
                        sum -= number;
                    } else {
                        throw std::runtime_error("unreachable");
                    }
                } else {
                    throw parser_error(lexer, "expected number");
                }
            } else {
                throw parser_error(lexer, "expected operator or end of line");
            }
        }
    };
}; // namespace protocol
