#include <AST.h>
#include <Printer.h>
#include <format>
namespace HKSL {

std::string unary_op_to_string(UnaryOp op) {
    switch(op) {
        case UnaryOp::Negate:
            return "-";
        default:
            HKSL_ERROR("Unimplemented");
    }
}
UnaryExpr::UnaryExpr(UnaryOp op, std::unique_ptr<Expr> expr) {
    this->op = op;
    this->expr = std::move(expr);
}
ExprKind UnaryExpr::kind() const {
    return ExprKind::UnaryExpr;
}

void UnaryExpr::print(Printer& printer) const {
    NodePrinter node("UnaryExpr", printer);
    node.field("op", unary_op_to_string(op));
    node.field("expr", expr.get());
}
std::string bin_op_to_string(BinOp op) {
    switch(op) {
            case BinOp::Add:
                return "+";
            case BinOp::Subtract:
                return "-";
            case BinOp::Multiply:
                return "*";
            case BinOp::Divide:
                return "/";
            case BinOp::Equals:
                return "==";
    }
}
BinExpr::BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    this->op = op;
    this->left = std::move(left);
    this->right = std::move(right);
}
ExprKind BinExpr::kind() const {
    return ExprKind::BinExpr;
}
void BinExpr::print(Printer& printer) const {
    NodePrinter node("BinExpr", printer);
    node.field("op", bin_op_to_string(op));
    node.field("left", left.get());
    node.field("right", right.get());
}

NumberConstant::NumberConstant(NumberLiteral literal) {
    this->number_literal = literal;
}
ExprKind NumberConstant::kind() const {
    return ExprKind::NumberConstant;
}

void NumberConstant::print(Printer& printer) const {
    printer.print(std::format("NumberConstant({})", number_literal.value));
}

Variable::Variable(const Identifier& name) {
    this->name = name;
}
ExprKind Variable::kind() const {
    return ExprKind::Variable;
}

void Variable::print(Printer& printer) const {
    printer.print(std::format("Variable({})", name.name));
}


ExprStatement::ExprStatement(std::unique_ptr<Expr> expr) {
    this->expr = std::move(expr);
}

StatementKind ExprStatement::kind() const {
    return StatementKind::Expr;
}

void ExprStatement::print(Printer& printer) const {
    NodePrinter node("ExprStatement", printer);
    node.field("expr", expr.get());
}
}