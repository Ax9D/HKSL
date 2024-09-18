#include "Util.h"
#include <Lexer.h>
#include <format>

namespace HKSL {
std::string Span::to_string() const {
    return std::format("Span {{ line: {}, col: {}}}", line, col);
}
std::string token_kind_to_debug_string(TokenKind token) {
  switch (token) {
  case TokenKind::Plus:
    return "Plus";
  case TokenKind::Minus:
    return "Minus";
  case TokenKind::Slash:
    return "Slash";
  case TokenKind::Star:
    return "Star";
  case TokenKind::Comma:
    return "Comma";
  case TokenKind::Dot:
    return "Dot";
  case TokenKind::Semicolon:
    return "Semicolon";
  case TokenKind::Equals:
    return "Equals";
  case TokenKind::LeftRound:
    return "LeftRound";
  case TokenKind::RightRound:
    return "RightRound";
  case TokenKind::LeftSquare:
    return "LeftSquare";
  case TokenKind::RightSquare:
    return "RightSquare";
  case TokenKind::LeftCurly:
    return "LeftCurly";
  case TokenKind::RightCurly:
    return "RightCurly";
  case TokenKind::DoubleEquals:
    return "DoubleEquals";
  case TokenKind::PlusEqual:
    return "PlusEqual";
  case TokenKind::MinusEqual:
    return "MinusEqual";
  case TokenKind::StarEqual:
    return "StarEqual";
  case TokenKind::SlashEqual:
    return "SlashEqual";
  case TokenKind::RightArrow:
    return "RightArrow";
  case TokenKind::Number:
    return "Number";
  case TokenKind::Identifier:
    return "Identifier";
  case TokenKind::KeywordIf:
    return "KeywordIf";
  case TokenKind::KeywordFn:
    return "KeywordFn";
  case TokenKind::KeywordLet:
    return "KeywordLet";
  case TokenKind::Eof:
    return "Eof";
  default:
    HKSL_UNREACHABLE();
  }
}

std::string token_kind_to_string(TokenKind token) {
  switch (token) {
  case TokenKind::Plus:
    return "+";
  case TokenKind::Minus:
    return "-";
  case TokenKind::Slash:
    return "/";
  case TokenKind::Star:
    return "*";
  case TokenKind::Comma:
    return ",";
  case TokenKind::Dot:
    return ".";
  case TokenKind::Semicolon:
    return ";";
  case TokenKind::Equals:
    return "=";
  case TokenKind::LeftRound:
    return "(";
  case TokenKind::RightRound:
    return ")";
  case TokenKind::LeftSquare:
    return "[";
  case TokenKind::RightSquare:
    return "]";
  case TokenKind::LeftCurly:
    return "{";
  case TokenKind::RightCurly:
    return "}";
  case TokenKind::DoubleEquals:
    return "==";
  case TokenKind::PlusEqual:
    return "+=";
  case TokenKind::MinusEqual:
    return "-=";
  case TokenKind::StarEqual:
    return "*=";
  case TokenKind::SlashEqual:
    return "/=";
  case TokenKind::RightArrow:
    return "->";
  case TokenKind::Number:
    return "Number";
  case TokenKind::Identifier:
    return "Identifier";
  case TokenKind::KeywordIf:
    return "if";
  case TokenKind::KeywordFn:
    return "fn";
  case TokenKind::KeywordLet:
    return "let";
  case TokenKind::Eof:
    return "Eof";
  default:
    HKSL_UNREACHABLE();
  }
}

std::string Token::to_string() const {
  std::string output;
  output += "Token { kind: ";
  output += token_kind_to_debug_string(kind);
  if (kind == TokenKind::Number) {
    output += std::format(" ({})", std::get<NumberLiteral>(data).value);
  } else if(kind == TokenKind::Identifier) {
    output += std::format("({})", std::get<Identifier>(data).name);
  }
  output += ", ";
  output += span.to_string();
  output += " }";

  
  return output;
}

const Identifier& Token::unwrap_identifier() const {
    if(kind == TokenKind::Identifier) {
        return std::get<Identifier>(data);
    } else {
        HKSL_ERROR("Variant is not an identifier");
    }
}
const NumberLiteral& Token::unwrap_number_literal() const {
    if(kind == TokenKind::Number) {
        return std::get<NumberLiteral>(data);
    } else {
        HKSL_ERROR("Variant is not a number literal");
    }
}

Lexer::Lexer(const char *code) {
  remaining = code;
  line = 1;
  col = 1;
}

char Lexer::current() { return *remaining; }
char Lexer::next() { return *(remaining + 1); }
void Lexer::advance() {
    if(current() == '\0') {
        HKSL_ERROR("Reached EOF while lexing");
    }

    if (current() == '\n') {
        line++;
        col = 1;
    } else {
        col++;
    }

    remaining++;
}
bool Lexer::matches(char ch) { 
    return current() == ch; 
}

bool Lexer::consume(char ch) {
    if (matches(ch)) {
        advance();
        return true;
    }

    return false;
}
bool Lexer::consume_two(char first, char second) {
   if (matches(first) && next() == second) {
        advance();
        advance();
        return true;
   } 

    return false;
}
bool Lexer::is_eof() { 
    return current() == '\0'; 
}

void Lexer::white_space() {
    while (matches('\n') || matches(' ')) {
        advance();
    }
}
void Lexer::skip_to_next_line() {
    while(matches('\n')) {
        advance();
    }
}
bool Lexer::is_digit(char ch) { 
    return ch >= '0' && ch <= '9'; 
}
bool Lexer::is_identifier_start(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <='Z') || (ch == '_');
}
bool Lexer::is_identifier(char ch) {
    return is_identifier_start(ch) || is_digit(ch);
}
double Lexer::to_digit(char ch) { 
    return ch - '0'; 
}
double Lexer::number_literal() {
    double value;

    double whole_part = 0;
    double fractional_part = 0;

    while (is_digit(current())) {
        double digit = to_digit(current());
        whole_part = whole_part * 10 + digit;
        advance();
    }

    if (consume('.')) {
        double divider = 10;
        while (is_digit(current())) {
        double digit = to_digit(current());
        fractional_part += digit / divider;
        divider *= 10;
        advance();
        }
    }

    return whole_part + fractional_part;
}
std::optional<TokenKind> Lexer::is_keyword(const std::string& identifier) {
    std::optional<TokenKind> ret = std::nullopt;

    if(identifier == "fn") {
        ret = TokenKind::KeywordFn;
    } else if(identifier == "if") {
        ret = TokenKind::KeywordIf;
    } 

    return ret;
}
std::pair<TokenKind, TokenData> Lexer::identifier() {
    std::string name;
    name+= current();
    advance();

    while(is_identifier(current())) {
        name += current();
        advance();
    }
    std::optional<TokenKind> keyword = is_keyword(name);

    if(keyword.has_value()) {
        return std::make_pair<TokenKind, TokenData>(std::move(keyword.value()), NoTokenData());
    } else {
        return std::make_pair<TokenKind, TokenData>(TokenKind::Identifier, Identifier {.name = name});
    };
}


Span Lexer::current_span() { return Span{.line = line, .col = col}; }

Token Lexer::token() {
    white_space();
    if(consume_two('/', '/')) {
        skip_to_next_line(); 
        white_space();
    } 
    Token token;
    token.span = current_span();

    if (is_eof()) {
        token.kind = TokenKind::Eof;
    } else if (consume_two('+', '=')) {
        token.kind = TokenKind::PlusEqual;
    } else if (consume_two('-', '=')) {
        token.kind = TokenKind::MinusEqual;
    } else if (consume_two('*', '=')) {
        token.kind = TokenKind::StarEqual;
    } else if (consume_two('/', '=')) {
        token.kind = TokenKind::SlashEqual;
    } else if (consume_two('=', '=')) {
        token.kind = TokenKind::DoubleEquals;
    }else if (consume_two('-', '>')) {
        token.kind = TokenKind::RightArrow;
    } else if (consume('+')) {
        token.kind = TokenKind::Plus;
    } else if (consume('-')) {
        token.kind = TokenKind::Minus;
    } else if (consume('*')) {
        token.kind = TokenKind::Star;
    } else if (consume('/')) {
        token.kind = TokenKind::Slash;
    } else if(consume(',')) {
        token.kind = TokenKind::Comma;
    } else if(consume('.')) {
        token.kind = TokenKind::Dot;
    } else if(consume(';')) {
        token.kind = TokenKind::Semicolon;
    } else if(consume('=')) {
        token.kind = TokenKind::Equals;
    } else if (consume('(')) {
        token.kind = TokenKind::LeftRound;
    } else if (consume(')')) {
        token.kind = TokenKind::RightRound;
    } else if (consume('[')) {
        token.kind = TokenKind::LeftSquare;
    } else if (consume(']')) {
        token.kind = TokenKind::RightSquare;
    } else if (consume('{')) {
        token.kind = TokenKind::LeftCurly;
    } else if (consume('}')) {
        token.kind = TokenKind::RightCurly;
    } else if (is_digit(current())) {
        double value = number_literal();
        token.kind = TokenKind::Number;
        token.data = NumberLiteral{.value = value};
    } else if (is_identifier_start(current())){
        auto [kind, data] = identifier();
        token.kind = kind;
        token.data = data;
    } else {
        HKSL_ERROR(std::format("Unexpected token: {}", current()));
    }

    return token;
}

std::vector<Token> Lexer::collect_tokens() {
    std::vector<Token> tokens;

    while(true) {
        auto toke = token();
        tokens.push_back(toke);
        if(toke.kind == TokenKind::Eof) {
            break;
        }
    }

    return tokens;
}
}