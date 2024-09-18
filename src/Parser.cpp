#include <Parser.h>
#include <format>

namespace HKSL {
Parser::Parser(const Token* tokens) {
    remaining = tokens;
}
const Token& Parser::current() const {
    return *remaining;
}
const Token& Parser::next() const {
    return *(remaining + 1);
}
void Parser::advance() {
    if(current().kind == TokenKind::Eof) {
        HKSL_ERROR("Reached EOF while parsing");
    }

    remaining++;
}
bool Parser::matches(TokenKind kind) const {
    return current().kind == kind;
}
bool Parser::consume(std::initializer_list<TokenKind> kinds, TokenKind* out_consumed) {
    for(auto kind: kinds) {
        if(consume(kind, out_consumed)) {
            return true;
        }
    }

    return false;
}
bool Parser::consume(TokenKind kind, TokenKind* out_consumed) {
    if(matches(kind)) {
        if(out_consumed) {
            *out_consumed = kind;
        }

        advance();

        return true;
    }

    return false;
}
void Parser::unexpected_token() {
    Span span = current().span;
    std::string token = token_kind_to_string(current().kind);
    HKSL_ERROR(std::format("Unexpected token: {} on line {}:{}", token, span.line, span.col));
}
void Parser::expect(TokenKind kind) {
    if(!consume(kind)) {
        Span span = current().span;
        std::string token = token_kind_to_string(kind);
        HKSL_ERROR(std::format("Expected {} on line {}:{}", token, span.line, span.col));
    }
}
std::unique_ptr<Statement> Parser::statement() {
    auto inner = expr();
    expect(TokenKind::Semicolon);

    return std::make_unique<ExprStatement>(std::move(inner));
}
std::unique_ptr<Expr> Parser::expr() {
    return equality();
}
std::unique_ptr<Expr> Parser::equality() {
    auto left = term();

    if(consume(TokenKind::DoubleEquals)) {
        auto right = term();
        BinOp op = BinOp::Equals;

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }

    return left;
}
std::unique_ptr<Expr> Parser::term() {
    auto left = factor();

    TokenKind op_token;
    while(consume({TokenKind::Plus, TokenKind::Minus}, &op_token)) {
        BinOp op;
        switch(op_token) {
            case TokenKind::Plus:
                op = BinOp::Add;
                break;
            case TokenKind::Minus:
                op = BinOp::Subtract;
                break;
            default:
                HKSL_UNREACHABLE();
        }

        auto right = factor();

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }

    return left;
}
std::unique_ptr<Expr> Parser::factor() {
    auto left = primary();

    TokenKind op_token;
    while(consume({TokenKind::Star, TokenKind::Slash}, &op_token)) {
        BinOp op;
        switch(op_token) {
            case TokenKind::Star:
                op = BinOp::Multiply;
                break;
            case TokenKind::Slash:
                op = BinOp::Divide;
                break;
            default:
                HKSL_UNREACHABLE();
        }

        auto right = primary();

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }

    return left;
}
std::unique_ptr<Expr> Parser::primary() {
    if(consume(TokenKind::LeftRound)) {
        auto inner = expr();
        expect(TokenKind::RightRound);
        return inner;
    }
    const Token& last = current();
    if(consume({TokenKind::Number, TokenKind::Identifier})) {
        switch(last.kind) {
            case TokenKind::Number:
                return std::make_unique<NumberConstant>(last.unwrap_number_literal());
            case TokenKind::Identifier:
                return std::make_unique<Variable>(last.unwrap_identifier());
            default:
                HKSL_UNREACHABLE();
        }
    }

    unexpected_token();
    return nullptr;
}
}