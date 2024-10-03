#pragma once
#include <string>
#include <Parse/Lexer.h>
#include <AST/Printer.h>

namespace HKSL {
struct ASTNode: ASTPrint {
    virtual ~ASTNode() = default;
};

enum class ExprKind {
    BinExpr,
    UnaryExpr,
    NumberConstant,
    Variable,
    CallExpr,
    AssignmentExpr,
    LetExpr,
};

bool expr_kind_is_place(ExprKind kind);

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
    void print(ASTPrinter& printer) const override;
};

struct BinExpr: public Expr {
    BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right);
    
    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;

    BinOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
};

struct NumberConstant: public Expr {
    NumberConstant(NumberLiteral literal);
    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;

    NumberLiteral number_literal;
};

struct Variable: public Expr {
    Variable(const Identifier& name);
    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;

    Identifier name;
};

using CallArgs = std::vector<std::unique_ptr<Expr>>;
struct CallExpr: public Expr {
    CallExpr(const Identifier& fn_name, std::vector<std::unique_ptr<Expr>>& args);
    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;

    Identifier fn_name;
    std::vector<std::unique_ptr<Expr>> args;
};

struct AssignmentExpr: public Expr {
    AssignmentExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);

    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;

    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;
};

struct LetExpr: public Expr {
    LetExpr(std::unique_ptr<Variable> variable, const std::optional<Identifier>& type, std::optional<std::unique_ptr<Expr>> rhs);

    std::unique_ptr<Variable> variable;
    std::optional<Identifier> type;
    std::optional<std::unique_ptr<Expr>> rhs;

    ExprKind kind() const override;
    void print(ASTPrinter& printer) const override;
};

enum class StatementKind {
    Expr,
    If,
    Else,
    Block,
    Function,
    Return,
};

struct Statement: public ASTNode {
    virtual ~Statement() = default;

    virtual StatementKind kind() const = 0;
};

struct ExprStatement: public Statement {
    ExprStatement(std::unique_ptr<Expr> expr);
    std::unique_ptr<Expr> expr;
    
    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;
};
struct BlockStatement: public Statement {
    BlockStatement(std::vector<std::unique_ptr<Statement>>& statements);
    std::vector<std::unique_ptr<Statement>> statements;

    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;
};

struct ElseStatement;

struct IfStatement: public Statement {
    IfStatement(std::unique_ptr<Expr> condtion, std::unique_ptr<BlockStatement> block, std::optional<std::unique_ptr<ElseStatement>> else_stmt);
    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;

    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStatement> then_block;
    std::optional<std::unique_ptr<ElseStatement>> else_stmt;
};

struct ElseStatement: public Statement {
    ElseStatement(std::unique_ptr<Statement> statement);
    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;

    std::unique_ptr<Statement> statement;
};

struct FunctionArg: ASTPrint {
    Identifier name;
    Identifier type;

    FunctionArg(const Identifier& name, const Identifier& type);

    void print(ASTPrinter& printer) const override; 
};
using FunctionArgs = std::vector<FunctionArg>;
struct Function: public Statement {
    Function(const Identifier& name, FunctionArgs&& args, std::unique_ptr<BlockStatement> block, const std::optional<Identifier>& return_type);
    Identifier name;
    FunctionArgs args;
    std::unique_ptr<BlockStatement> block;
    std::optional<Identifier> return_type;

    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;
};

struct ReturnStatement: public Statement {
    ReturnStatement(std::optional<std::unique_ptr<Expr>> value);
    
    StatementKind kind() const override;
    void print(ASTPrinter& printer) const override;

    std::optional<std::unique_ptr<Expr>> value;
};


struct AST: ASTNode {
    public:
        AST(std::vector<std::unique_ptr<Statement>>& statements);
        friend class ASTPrinter;
        friend class Visitor;
        void print(ASTPrinter& printer) const override;
    private:
        std::vector<std::unique_ptr<Statement>> statements;
};
}