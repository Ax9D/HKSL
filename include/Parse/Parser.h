#pragma once

#include <Context.h>
#include <AST/AST.h>
#include <Parse/Lexer.h>

namespace HKSL {

class Parser {
    public:
        Parser(CompilationContext&, const Token* tokens);
        std::unique_ptr<AST> program();
        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> expr_statement();
        std::unique_ptr<BlockStatement> block();
        std::unique_ptr<Statement> function();
        FunctionArgs function_args();
        std::unique_ptr<Statement> return_statement();
        std::unique_ptr<Statement> if_statement();
        std::unique_ptr<ElseStatement> else_statement();
        
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
        std::unique_ptr<VarDecl> var_decl();
        Identifier identifier();
        Type* type();
    private:
        const Token& current() const;
        const Token& next() const;
        void advance();
        bool is_eof();
        bool matches(TokenKind kind) const;
        bool consume(std::initializer_list<TokenKind> kinds, Token* out_consumed = nullptr);
        bool consume(TokenKind kind, Token* out_consumed = nullptr);  
        void unexpected_token();
        void expect(TokenKind kind, Token* out_consumed = nullptr, const char* error = nullptr);
        const Token* remaining;
        CompilationContext& context;
};

}