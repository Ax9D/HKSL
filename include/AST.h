#pragma once
#include <string>
#include <Lexer.h>

namespace HKSL {
enum class ExprKind {
    BinExpr,
    NumberConstant,
    Variable,
    CallExpr,
    AssignmentExpr,
};

struct Expr {
    virtual ~Expr() = default;

    virtual ExprKind kind() = 0;
    virtual std::string to_string() = 0;
};

enum class BinOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Equals,
};

struct BinExpr: public Expr {
    BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    
    ExprKind kind() override;
    std::string to_string() override; 

    BinOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct NumberConstant: public Expr {
    NumberConstant(NumberLiteral literal);
    ExprKind kind() override;
    std::string to_string() override;

    NumberLiteral number_literal;
};

struct Variable: public Expr {
    Variable(const Identifier& name);
    ExprKind kind() override;
    std::string to_string() override;

    Identifier name;
};

struct CallExpr: public Expr {
    CallExpr(Identifier fn_name, const std::vector<std::unique_ptr<Expr>>& args);

    Identifier fn_name;
    std::vector<std::unique_ptr<Expr>> args;
};

struct AssignmentExpr: public Expr {
    AssignmentExpr(Identifier target);

    Identifier target;
    std::unique_ptr<Expr> rhs;
};

enum class StatementKind {
    Expr,
    If,
    Else,
    Function,
    VarDeclartion,
};

struct Statement {
    virtual ~Statement() = default;

    virtual StatementKind kind() = 0;
    virtual std::string to_string() = 0;
};

struct ExprStatement: public Statement {
    ExprStatement(std::unique_ptr<Expr> expr);
    std::unique_ptr<Expr> expr;
    
    StatementKind kind() override;
    std::string to_string() override;
};

struct IfStatement: public Statement {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Statement> else_part = nullptr;

    StatementKind kind() override;
    std::string to_string() override;
};

struct ElseStatement: public Statement {
    std::unique_ptr<Statement> statement;

    StatementKind kind() override;
    std::string to_string() override;
};

}