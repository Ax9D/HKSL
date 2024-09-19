#pragma once

#include <AST.h>
#include <Lexer.h>

namespace HKSL {

class Parser {
    public:
        Parser(const Token* tokens);
        std::vector<std::unique_ptr<Statement>> program();
        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> expr_statement();
        std::unique_ptr<BlockStatement> block();
        std::unique_ptr<Statement> function();
        std::vector<FunctionArg> function_args();
        std::unique_ptr<Statement> return_statement();
        std::unique_ptr<Expr> let();
        std::unique_ptr<Expr> assignment();
        std::unique_ptr<Expr> expr();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> primary();
        std::unique_ptr<Expr> place();
        std::unique_ptr<Expr> call_expr();
        std::unique_ptr<Variable> variable();
        Identifier identifier();
        Identifier type();
    private:
        const Token& current() const;
        const Token& next() const;
        void advance();
        bool is_eof();
        bool matches(TokenKind kind) const;
        bool consume(std::initializer_list<TokenKind> kinds, TokenKind* out_consumed = nullptr);
        bool consume(TokenKind kind, TokenKind* out_consumed = nullptr);  
        void unexpected_token();
        void expect(TokenKind kind, const char* error = nullptr);

        const Token* remaining;
};

}