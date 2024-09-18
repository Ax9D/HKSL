#pragma once
#include <string>
#include <Lexer.h>
#include <Printer.h>

namespace HKSL {

struct ASTNode: Print {
    virtual ~ASTNode() = default;
};

enum class ExprKind {
    BinExpr,
    UnaryExpr,
    NumberConstant,
    Variable,
    CallExpr,
    AssignmentExpr,
};

struct Expr: public ASTNode {
    virtual ~Expr() = default;
    virtual ExprKind kind() const = 0;
};

enum class UnaryOp {
    Negate
};

std::string unary_op_to_string(UnaryOp op);

enum class BinOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Equals,
};

std::string bin_op_to_string(BinOp op);

struct UnaryExpr: public Expr {
    UnaryExpr(UnaryOp op, std::unique_ptr<Expr> expr);

    UnaryOp op;
    std::unique_ptr<Expr> expr;

    ExprKind kind() const override;
    void print(Printer& printer) const override;
};

struct BinExpr: public Expr {
    BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    
    ExprKind kind() const override;
    void print(Printer& printer) const override;

    BinOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct NumberConstant: public Expr {
    NumberConstant(NumberLiteral literal);
    ExprKind kind() const override;
    void print(Printer& printer) const override;

    NumberLiteral number_literal;
};

struct Variable: public Expr {
    Variable(const Identifier& name);
    ExprKind kind() const override;
    void print(Printer& printer) const override;

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

struct Statement: public ASTNode {
    virtual ~Statement() = default;

    virtual StatementKind kind() const = 0;
};

struct ExprStatement: public Statement {
    ExprStatement(std::unique_ptr<Expr> expr);
    std::unique_ptr<Expr> expr;
    
    StatementKind kind() const override;
    void print(Printer& printer) const override;
};

struct IfStatement: public Statement {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Statement> else_part = nullptr;

    StatementKind kind() const override;
    void print(Printer& printer) const override;
};

struct ElseStatement: public Statement {
    std::unique_ptr<Statement> statement;

    StatementKind kind() const override;
    void print(Printer& printer) const override;
};

class AST {
    public:
        AST();
    friend class Parser;
    private:
        std::vector<std::unique_ptr<Statement>> statements;
};
}