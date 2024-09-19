#pragma once 
#include <string>
#include <vector>
#include <variant>
#include <optional>

#include <Util.h>

namespace HKSL {

struct Span {
    uint32_t line;
    uint32_t col;
    std::string to_string() const;
};
enum class TokenKind {
    Plus, 
    Minus,
    Slash,
    Star,
    Comma,
    Dot,
    Colon,
    Semicolon,
    Equals,
    LeftSquare,
    RightSquare,
    LeftCurly,
    RightCurly,
    LeftRound,
    RightRound,

    DoubleEquals,
    PlusEqual,
    MinusEqual,
    StarEqual,
    SlashEqual,
    RightArrow,

    Number,
    Identifier,
    KeywordIf,
    KeywordElse,
    KeywordFn,
    KeywordLet,
    KeywordReturn,
    Eof,
};

std::string token_kind_to_debug_string(TokenKind token);
std::string token_kind_to_string(TokenKind token);

struct Identifier {
    std::string name;
};
struct NumberLiteral {
    double value;
};
struct NoTokenData {};
using TokenData = std::variant<Identifier, NumberLiteral, NoTokenData>;

struct Token {
    TokenKind kind;
    TokenData data;
    Span span;
    const Identifier& unwrap_identifier() const;
    const NumberLiteral& unwrap_number_literal() const;
    std::string to_string() const;
};

class Lexer {
    public:
        Lexer(const char* code);
        Lexer(const Lexer& other) = default;
        Lexer& operator=(Lexer& other) = default;
        bool is_eof();
        Token token();
        std::vector<Token> collect_tokens();
    private:
        char current();
        char next();
        void advance();
        bool matches(char ch);
        bool consume(char ch);
        bool consume_two(char first, char second);
        void white_space();
        void skip_to_next_line();
        bool is_digit(char ch);
        bool is_identifier_start(char ch);
        bool is_identifier(char ch);
        std::optional<TokenKind> is_keyword(const std::string& identifier);
        std::pair<TokenKind, TokenData> identifier();
        double to_digit(char ch);
        double number_literal();
        Span current_span();
    private:
        const char* remaining;
        uint32_t line;
        uint32_t col;
};
}