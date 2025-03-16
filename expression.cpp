#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <memory>
#include <cassert>
#include <vector>
#include <format>

#define COMPILE_ERROR(message) do\
{\
    std::cout << (message) << std::endl; \
} while(0)\

#define ERROR(message) do\
{\
    std::cout << (message) << std::endl;\
    std::exit(-1);\
} while(0)\

#define UNREACHABLE() ERROR("Unreachable")

enum class BinOp {
    Add,
    Subtract,
    Divide,
    Multiply,
};

class Expr {
    public:
        virtual ~Expr() = default;
        double virtual eval() = 0;
};

class UnaryExpr: public Expr {};

struct Number: public Expr {
    Number(double value) {
        this->value = value;
    }
    double eval() {
        return value;
    }
    double value;
};

std::unique_ptr<Expr> make_number(double value) {
    return std::make_unique<Number>(value);
}

struct BinExpr: public Expr {
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinOp op;
    BinExpr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, BinOp op) {
        this->left = std::move(left);
        this->right = std::move(right);
        this->op = op;
    }
    double eval() {
        double left_value = left->eval();
        double right_value = right->eval();

        switch(op) {
            case BinOp::Add: return left_value + right_value;
            case BinOp::Subtract: return left_value - right_value;
            case BinOp::Multiply: return left_value * right_value;
            case BinOp::Divide: return left_value / right_value;
        }
    }
};

std::unique_ptr<Expr> make_binary_expr(std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, BinOp op) {
    return std::make_unique<BinExpr>(std::move(left), std::move(right), op);
}

struct Span {
    uint32_t line;
    uint32_t col;
};
enum class TokenKind {
    Plus, 
    Minus,
    Slash,
    Star,
    LeftRound,
    RightRound,
    Number,
    Eof,
};

std::string token_kind_to_string(TokenKind token) {
    switch (token) {
        case TokenKind::Plus:       return "Plus";
        case TokenKind::Minus:      return "Minus";
        case TokenKind::Slash:      return "Slash";
        case TokenKind::Star:       return "Star";
        case TokenKind::LeftRound:  return "LeftRound";
        case TokenKind::RightRound: return "RightRound";
        case TokenKind::Number:     return "Number";
        case TokenKind::Eof:        return "Eof";
        default:                    return "Unknown";
    }
}
struct NumberLiteral {
    double value;
};
union TokenData {
    NumberLiteral number;
};
struct Token {
    TokenKind kind;
    TokenData data;
    Span span;

    std::string to_string() {
        std::string output;
        output += "{";
        output += token_kind_to_string(kind);
        if(kind == TokenKind::Number) {
            output += std::format("({})", data.number.value);
        }
        output += ",";
        output += std::format(" Span({}, {})", span.line, span.col);
        output += "}";

        return output;
    }
};
class Lexer {
    public:
        Lexer(const char* code) {
            remaining = code;
            line = 1;
            col = 0;
        }
        Lexer(const Lexer& other) = default;
        Lexer& operator=(Lexer& other) = default;

        char current() {
            return *remaining;
        }
        char next() {
            return *(remaining + 1);
        }
        void advance() {
            if(current() == '\n') {
                line++;
                col = 0;
            } else {
                col++;
            }

            remaining++;
        }
        bool matches(char ch) {
            return current() == ch;
        }
        bool consume(char ch) {
            if(matches(ch)) {
                advance();
                return true;
            }

            return false;
        }
        bool is_eof() {
            return current() == '\0';
        }
        void white_space() {
            while(matches('\n') || matches(' ')) {
                advance();
            }
        }
        bool is_digit(char ch) {
            return current() >= '0' && current() <= '9';
        }
        double to_digit(char ch) {
            return ch - '0';
        }
        double number_literal() {
            double value;

            double whole_part = 0;
            double fractional_part = 0;

            while(is_digit(current())) {
                double digit = to_digit(current());
                whole_part = whole_part * 10 + digit;
                advance(); 
            }

            if(consume('.')) {
                double divider = 10;
                while(is_digit(current())) {
                    double digit = to_digit(current());
                    fractional_part += digit / divider;
                    divider *= 10; 
                    advance();
                }
            }

            return whole_part + fractional_part;
        }
        Span current_span() {
            return Span {
                .line = line,
                .col = col
            };
        }
        Token token() {
            white_space();
            char ch = current();
            Token token;
            token.span = current_span();
            if(is_eof()) {
                token.kind = TokenKind::Eof;
            } else if(consume('+')) {
                token.kind = TokenKind::Plus;
            } else if(consume('-')) {
                token.kind = TokenKind::Minus;
            } else if(consume('*')) {
                token.kind = TokenKind::Star;
            } else if(consume('/')) {
                token.kind = TokenKind::Slash;
            } else if (consume('(')) {
                token.kind = TokenKind::LeftRound;
            } else if (consume(')')) {
                token.kind = TokenKind::RightRound;
            }
            else if(ch >= '0' && ch <= '9') {
                double value = number_literal();
                token.kind = TokenKind::Number;
                token.data.number = NumberLiteral {.value = value};
            } else {
                std::cout << "Unrecognized token: " << ch << std::endl;
                std::exit(-1);
            }

            return token;
        }
    private:
        const char* remaining;
        uint32_t line;
        uint32_t col;
};
class AST {
    public:

};
class Parser {
    public:
        Parser(const Token* tokens) {
            remaining = tokens;
        }
        const Token& current() {
            return *remaining;
        }
        const Token& next() {
            return *(remaining + 1);
        }
        bool is_eof() {
            return current().kind == TokenKind::Eof;
        }
        void advance() {
            if(is_eof()) {
                ERROR(std::format("Reached EOF while parsing {}:{}", current().span.line, current().span.col));
            }
            remaining = remaining + 1;
        }
        bool matches(TokenKind kind) {
            return current().kind == kind; 
        }
        bool consume(const std::initializer_list<TokenKind>& kinds, TokenKind* matched_out = nullptr) {
            for(auto kind: kinds) {
                if(matches(kind)) {
                    if(matched_out) {
                        *matched_out = kind;
                    }
                    advance();
                    return true;
                }
            }

            return false;
        }
        bool consume(const TokenKind& kind, TokenKind* matched_out = nullptr) {
            return consume({kind}, matched_out);
        }
        void expect(TokenKind kind) {
            if(!consume({kind})) {
                ERROR(std::format("Expected {} at {}:{}", token_kind_to_string(kind), current().span.line, current().span.col));
            }
        }
        // primary = "(" expr | number ")"
        std::unique_ptr<Expr> primary() {
            if(consume(TokenKind::LeftRound)) {
                std::unique_ptr<Expr> inner = expr();
                expect(TokenKind::RightRound);

                return inner;
            } else if(matches(TokenKind::Number)) {
                double value = current().data.number.value;
                advance();
                return make_number(value);
            } else {
                ERROR(std::format("Unexpected token: {} at {}:{}", token_kind_to_string(current().kind), current().span.line, current().span.col));
            }
        }
        // factor = primary ("*" | "/") primary
        std::unique_ptr<Expr> factor() {
            std::unique_ptr<Expr> left = primary();
            TokenKind op_token;
            while(consume({TokenKind::Star, TokenKind::Slash}, &op_token)) {
                std::unique_ptr<Expr> right = primary();

                BinOp op;
                switch(op_token) {
                    case TokenKind::Star: 
                            op = BinOp::Multiply;
                            break;
                    case TokenKind::Slash:
                            op = BinOp::Divide;
                            break;
                    default:
                        UNREACHABLE();

                }
                left = std::move(make_binary_expr(std::move(left), std::move(right), op));
            }
            return left;
        }
        // term = factor ("+" | "-") factor 
        std::unique_ptr<Expr> term() {
            std::unique_ptr<Expr> left = factor();
            TokenKind op_token;
            while(consume({TokenKind::Plus, TokenKind::Minus}, &op_token)) {
                std::unique_ptr<Expr> right = factor();

                BinOp op;
                switch(op_token) {
                    case TokenKind::Plus: 
                            op = BinOp::Add;
                            break;
                    case TokenKind::Minus:
                            op = BinOp::Subtract;
                            break;
                    default:
                        UNREACHABLE();

                }
                left = std::move(make_binary_expr(std::move(left), std::move(right), op));
            }
            return left;
        }
        std::unique_ptr<Expr> expr() {
            return term();
        }
    private:
        const Token* remaining;
};
std::string read_to_string(const char* path) {
    FILE* file = fopen(path, "r");

    if(!file) {
        std::cout << "Couldn't read file: " << strerror(errno) << std::endl;
        std::exit(-1);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<char> data(size, '\0');
    assert(fread(data.data(), sizeof(char), size, file) == size);
    fclose(file);

    return std::string(data.begin(), data.end());
}

int main(int argc, const char** argv) {
    assert(argc == 2);
    std::string code = read_to_string(argv[1]);
    std::cout << code << std::endl;

    Lexer lexer(code.c_str());
    std::vector<Token> tokens;
    while(true) {
        Token token = lexer.token();
        tokens.push_back(token);
        if(lexer.is_eof()) {
            break;
        }

        // std::cout << token.to_string() << std::endl;
    }
    // auto expr = make_binary_expr(make_binary_expr(make_number(1.0), make_number(2.0),BinOp::Add), make_number(6.0), BinOp::Multiply);

    Parser parser(tokens.data());
    std::unique_ptr<Expr> expr = parser.expr();
    std::cout << expr->eval() << std::endl;

    return 0;
}