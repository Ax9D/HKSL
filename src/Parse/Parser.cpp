#include "AST.h"
#include <Parse/Parser.h>
#include <format>

namespace HKSL {
Parser::Parser(CompilationContext& _context, const Token* tokens): context(_context) {
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
bool Parser::consume(std::initializer_list<TokenKind> kinds, Token* out_consumed) {
    for(auto kind: kinds) {
        if(consume(kind, out_consumed)) {
            return true;
        }
    }

    return false;
}
bool Parser::consume(TokenKind kind, Token* out_consumed) {
    if(matches(kind)) {
        if(out_consumed) {
            *out_consumed = current();
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
void Parser::expect(TokenKind kind, Token* out_consumed, const char* error) {
    if(!consume(kind, out_consumed)) {
        Span span = current().span;
        std::string token = token_kind_to_string(kind);
        if(error) {
            HKSL_ERROR(std::format("{} on line {}:{}", error, span.line, span.col));  
        } else {
            HKSL_ERROR(std::format("Expected {} on line {}:{}", token, span.line, span.col));
        }
    }
}
std::unique_ptr<AST> Parser::program() {
    std::vector<std::unique_ptr<Statement>> statements;

    while(!is_eof()) {
        statements.push_back(statement());
    }

    return std::make_unique<AST>(statements);
}
std::unique_ptr<Statement> Parser::statement() {
    if(matches(TokenKind::KeywordFn)) {
        return function();
    } else if(matches(TokenKind::LeftCurly)) {
        return block();
    } else if(matches(TokenKind::KeywordReturn)){
        return return_statement();
    } else if (matches(TokenKind::KeywordIf)) {
        return if_statement();
    } else {
        return expr_statement();
    }
}
std::unique_ptr<Statement> Parser::return_statement() {
    Token ret_token;
    expect(TokenKind::KeywordReturn, &ret_token);

    auto ret = std::make_unique<ReturnStatement>(std::nullopt, ret_token);

    if(!consume(TokenKind::Semicolon)) {
        ret->value = expr();
        expect(TokenKind::Semicolon);
    }

    return std::move(ret);
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

    return std::make_unique<BlockStatement>(inner_statements);
}
std::unique_ptr<Statement> Parser::function() {
    expect(TokenKind::KeywordFn);
    
    const auto name = identifier();

    FunctionArgs args = function_args();

    Type* return_type = context.type_registry().get_void();

    if(consume(TokenKind::RightArrow)) {
        return_type = type();
    }

    std::unique_ptr<BlockStatement> block_stmt = block();

    return std::make_unique<Function>(name, args, std::move(block_stmt), return_type);
}
FunctionArgs Parser::function_args() {
    FunctionArgs args;
    expect(TokenKind::LeftRound);

    while(true) {
        const auto& maybe_name= current();
        if(consume(TokenKind::Identifier)) {
            const auto& name = maybe_name.unwrap_identifier();
            expect(TokenKind::Colon);

            auto ty = std::make_optional<Type*>(type());

            args.push_back(VarDecl(name, ty));

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
std::unique_ptr<Statement> Parser::if_statement() {
    expect(TokenKind::KeywordIf);
    auto condition = expr();

    {
    ASTPrinter printer;
    condition->print(printer);
    }
    auto block_stmt = block();
    {
        ASTPrinter printer;
        block_stmt->print(printer);
    }
    std::optional<std::unique_ptr<ElseStatement>> else_stmt = std::nullopt;
    if(matches(TokenKind::KeywordElse)) {
        else_stmt = else_statement();
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(block_stmt), std::move(else_stmt));
}
std::unique_ptr<ElseStatement> Parser::else_statement() {
    expect(TokenKind::KeywordElse);
    std::unique_ptr<Statement> statement;
    if(matches(TokenKind::KeywordIf)) {
        // else if
        statement = if_statement();
    } else {
        statement = block();
    }

    return std::make_unique<ElseStatement>(std::move(statement));
}
std::unique_ptr<Expr> Parser::expr() {
    return let();
}

std::unique_ptr<Expr> Parser::let() {
    if(consume(TokenKind::KeywordLet)) {
        auto var_decl_expr = var_decl();
        auto let_expr = std::make_unique<LetExpr>(std::move(var_decl_expr), std::nullopt, std::nullopt);
        
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

    Token op_token;
    if(consume(TokenKind::Equals, &op_token)) {

        if(!expr_kind_is_place(lhs->kind())) {
            HKSL_ERROR(std::format("Target of assignment can only be a variable, found: {}: {}:{}", expr_kind_to_string(lhs->kind()), op_token.span.line, op_token.span.col));
        }
        std::unique_ptr<Expr> rhs = assignment();
        
        lhs = std::make_unique<AssignmentExpr>(std::move(lhs), std::move(rhs), op_token);
    }

    return lhs;
}
std::unique_ptr<Expr> Parser::equality() {
    auto left = term();
    Token op_token;
    if(consume(TokenKind::DoubleEquals, &op_token)) {
        auto right = term();
        BinOp op = BinOp::Equals;

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right), op_token);
    }

    return left;
}
std::unique_ptr<Expr> Parser::term() {
    auto left = factor();

    Token op_token;
    while(consume({TokenKind::Plus, TokenKind::Minus}, &op_token)) {
        BinOp op;
        switch(op_token.kind) {
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

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right), op_token);
    }

    return left;
}
std::unique_ptr<Expr> Parser::factor() {
    auto left = unary();

    Token op_token;
    while(consume({TokenKind::Star, TokenKind::Slash}, &op_token)) {
        BinOp op;
        switch(op_token.kind) {
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

        left = std::make_unique<BinExpr>(op, std::move(left), std::move(right), op_token);
    }

    return left;
}
std::unique_ptr<Expr> Parser::unary() {
    Token op_token;
    if(consume(TokenKind::Minus, &op_token)) {
        std::unique_ptr<Expr> inner_expr = unary();
        return std::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(inner_expr), op_token);
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
        return call_expr();
    }

    unexpected_token();
    return nullptr;
}
std::unique_ptr<Expr> Parser::call_expr() {
    auto name = identifier();
    if(consume(TokenKind::LeftRound)) {

        CallArgs args;
        while(true) {
            if(!consume(TokenKind::RightRound)) {
                auto arg = expr();
                args.push_back(std::move(arg));
                if(!consume(TokenKind::Comma)) {
                    expect(TokenKind::RightRound);
                    break;
                }
            } else {
                break;
            }
        }
        return std::make_unique<CallExpr>(name, args);
    } else {
        return std::make_unique<Variable>(name);
    }
}
std::unique_ptr<Variable> Parser::variable() {
    return std::make_unique<Variable>(identifier());
}
std::unique_ptr<VarDecl> Parser::var_decl() {
    auto name = identifier();

    std::optional<Type*> type_ = std::nullopt;
    if(consume(TokenKind::Colon)) {
        // Is explictly typed
        type_ = type();
    }

    return std::make_unique<VarDecl>(name, type_);
}
Identifier Parser::identifier() {
    const auto& maybe_identifier = current();
    expect(TokenKind::Identifier);
    return maybe_identifier.unwrap_identifier();
}
Type* Parser::type() {
    const auto& maybe_identifier = current();
    expect(TokenKind::Identifier, nullptr, "Expected type");
    auto type_name = maybe_identifier.unwrap_identifier();

    auto type = context.type_registry().get(type_name.name);

    if(!type) {
        HKSL_ERROR(std::format("Unknown type: {}", type_name.name));
    }

    return type;
}
}