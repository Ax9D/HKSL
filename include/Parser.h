#pragma once

#include <AST.h>
#include <Lexer.h>

namespace HKSL {

class Parser {
    public:
        Parser(const Token* tokens);
        std::unique_ptr<Statement> statement();
        std::unique_ptr<Expr> expr();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> primary();
    private:
        const Token& current() const;
        const Token& next() const;
        void advance();
        bool matches(TokenKind kind) const;
        bool consume(std::initializer_list<TokenKind> kinds, TokenKind* out_consumed = nullptr);
        bool consume(TokenKind kind, TokenKind* out_consumed = nullptr);  
        void unexpected_token();
        void expect(TokenKind kind);

        const Token* remaining;
};

}