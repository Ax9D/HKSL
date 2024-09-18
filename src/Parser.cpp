#include "AST.h"
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
bool Parser::is_eof() {
    return current().kind == TokenKind::Eof;
}
void Parser::advance() {
    if(is_eof()) {
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
void Parser::expect(TokenKind kind, const char* error) {
    if(!consume(kind)) {
        Span span = current().span;
        std::string token = token_kind_to_string(kind);
        if(error) {
            HKSL_ERROR(std::format("{} on line {}:{}", error, span.line, span.col));  
        } else {
            HKSL_ERROR(std::format("Expected {} on line {}:{}", token, span.line, span.col));
        }
    }
}
std::vector<std::unique_ptr<Statement>> Parser::program() {
    std::vector<std::unique_ptr<Statement>> statements;

    while(!is_eof()) {
        statements.push_back(statement());
    }

    return statements;
}
std::unique_ptr<Statement> Parser::statement() {
    if(matches(TokenKind::KeywordFn)) {
        return function();
    } else if(matches(TokenKind::LeftCurly)) {
        return block();
    } else {
        return expr_statement();
    }
}
std::unique_ptr<Statement> Parser::expr_statement() {
    auto inner = expr();
    expect(TokenKind::Semicolon);

    return std::make_unique<ExprStatement>(std::move(inner));
}
std::unique_ptr<BlockStatement> Parser::block() {
    expect(TokenKind::LeftCurly);
    std::vector<std::unique_ptr<Statement>> inner_statements;

    while(!consume(TokenKind::RightCurly)) {
        inner_statements.push_back(statement());
    }

    return std::make_unique<BlockStatement>(std::move(inner_statements));
}
std::unique_ptr<Statement> Parser::function() {
    expect(TokenKind::KeywordFn);
    
    const auto& maybe_name = current();
    expect(TokenKind::Identifier);
    const auto& name = maybe_name.unwrap_identifier();

    FunctionArgs args = function_args();

    std::optional<Identifier> return_type = std::nullopt;

    if(consume(TokenKind::RightArrow)) {
        return_type = type();
    }

    
    std::unique_ptr<BlockStatement> block_stmt = block();

    return std::make_unique<Function>(name, std::move(args), std::move(block_stmt), return_type);
}
FunctionArgs Parser::function_args() {
    FunctionArgs args;
    expect(TokenKind::LeftRound);

    while(true) {
        const auto& maybe_name= current();
        if(consume(TokenKind::Identifier)) {
            const auto& name = maybe_name.unwrap_identifier();
            expect(TokenKind::Colon);

            auto ty = type();

            args.push_back(FunctionArg(name, ty));

            if(!consume(TokenKind::Comma)) {
                break;
            }
        } else {
            break;
        }
    }

    expect(TokenKind::RightRound);

    return args;
}
std::unique_ptr<Expr> Parser::expr() {
    return let();
}

std::unique_ptr<Expr> Parser::let() {
    if(consume(TokenKind::KeywordLet)) {
        auto variable_expr = variable();
        auto let_expr = std::make_unique<LetExpr>(std::move(variable_expr), std::nullopt, nullptr);

        if(consume(TokenKind::Colon)) {
            // Is explictly typed
            let_expr->type = type();
        }
        if(consume(TokenKind::Equals)) {
            auto rhs = expr();
            let_expr->rhs = std::move(rhs);
        }

        return let_expr;
    }

    return assignment();
}
std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> lhs = equality();

    const Token& last = current();
    if(consume(TokenKind::Equals)) {
        if(!expr_kind_is_place(lhs->kind())) {
            HKSL_ERROR(std::format("Target of assignment can only be a variable: {}:{}", last.span.line, last.span.col));
        }
        std::unique_ptr<Expr> rhs = assignment();
        
        lhs = std::make_unique<AssignmentExpr>(std::move(lhs), std::move(rhs));
    }

    return lhs;
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
    auto left = unary();

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

        auto right = unary();

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right));
    }

    return left;
}
std::unique_ptr<Expr> Parser::unary() {
    if(consume(TokenKind::Minus)) {
        std::unique_ptr<Expr> inner_expr = unary();
        return std::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(inner_expr));
    } 

    return primary();
}
std::unique_ptr<Expr> Parser::primary() {
    if(consume(TokenKind::LeftRound)) {
        auto inner = expr();
        expect(TokenKind::RightRound);
        return inner;
    }
    const Token& last = current();
    if(consume(TokenKind::Number)) {
        return std::make_unique<NumberConstant>(last.unwrap_number_literal());
    }

    return place();
}

std::unique_ptr<Expr> Parser::place() {
    if(matches(TokenKind::Identifier)) {
        return variable();
    }

    //TODO: CallExpr

    unexpected_token();
    return nullptr;
}
std::unique_ptr<Variable> Parser::variable() {
    const Token& last = current();
    expect(TokenKind::Identifier);
    return std::make_unique<Variable>(last.unwrap_identifier());
}
Identifier Parser::type() {
    const auto& maybe_identifier = current();
    expect(TokenKind::Identifier, "Expected type");
    return maybe_identifier.unwrap_identifier();
}
}