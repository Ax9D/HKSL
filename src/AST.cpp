#include <AST.h>
#include <format>
namespace HKSL {

BinExpr::BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right) {
    this->op = op;
    this->left = std::move(left);
    this->right = std::move(right);
}
ExprKind BinExpr::kind() {
    return ExprKind::BinExpr;
}
std::string BinExpr::to_string() {
    return std::format("BinExpr {{ left: {}, right: {}}}", left->to_string(), right->to_string());
}

NumberConstant::NumberConstant(NumberLiteral literal) {
    this->number_literal = literal;
}
ExprKind NumberConstant::kind() {
    return ExprKind::NumberConstant;
}

std::string NumberConstant::to_string() {
    return std::format("NumberConstant({})", number_literal.value);
}

Variable::Variable(const Identifier& name) {
    this->name = name;
}
ExprKind Variable::kind() {
    return ExprKind::Variable;
}

std::string Variable::to_string() {
    return std::format("Variable({})", name.name);
}


ExprStatement::ExprStatement(std::unique_ptr<Expr> expr) {
    this->expr = std::move(expr);
}

StatementKind ExprStatement::kind() {
    return StatementKind::Expr;
}

std::string ExprStatement::to_string() {
    return std::format("ExprStatement({})", expr->to_string());
}
}