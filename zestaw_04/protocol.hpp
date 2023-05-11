#include "net.hpp"
#include <stdexcept>
#include <string>
#include <variant>

namespace protocol {

    class Lexer;
    class LexerError;

    enum OperatorKind {
        Add,
        Sub,
    };

    struct TokenEndl {};

    struct TokenOperator {
        OperatorKind kind;
    };

    struct TokenNumber {
        uint32_t value;
    };

    using Token = std::variant<TokenEndl, TokenOperator, TokenNumber>;

    class Lexer {
      private:
        net::StremReader m_reader;
        int m_cursor = 0;
        int m_line_begin = 0;
        int m_line_number = 1;
        uint8_t peek();
        uint8_t read();

      public:
        Lexer(net::StremReader reader);
        Token next();
        Token next_line();
        int cursor() const;
        int line() const;
        int column() const;
    };

    std::runtime_error lexer_error(const Lexer &lexer, std::string message = "");

    std::runtime_error parser_error(const Lexer &lexer, std::string message = "");

    struct ParsedLineValue {
        int32_t value;
    };

    struct ParsedLineError {
        std::runtime_error error;
    };

    struct ParsedLineEmpty {};

    using ParsedLine = std::variant<ParsedLineValue, ParsedLineEmpty, ParsedLineError>;

    ParsedLine parse_line(Lexer &lexer);

} // namespace protocol
