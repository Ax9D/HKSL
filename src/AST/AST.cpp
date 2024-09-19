#include <AST/AST.h>
#include <format>
namespace HKSL {
bool expr_kind_is_place(ExprKind kind) {
    return kind == ExprKind::Variable || kind == ExprKind::CallExpr;
}

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

void UnaryExpr::print(ASTPrinter& printer) const {
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
void BinExpr::print(ASTPrinter& printer) const {
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

void NumberConstant::print(ASTPrinter& printer) const {
    printer.print(std::format("NumberConstant({})", number_literal.value));
}

Variable::Variable(const Identifier& name) {
    this->name = name;
}
ExprKind Variable::kind() const {
    return ExprKind::Variable;
}

void Variable::print(ASTPrinter& printer) const {
    printer.print(std::format("Variable({})", name.name));
}
CallExpr::CallExpr(const Identifier& fn_name, std::vector<std::unique_ptr<Expr>>& args) {
    this->fn_name = fn_name;
    this->args = std::move(args);
}
ExprKind CallExpr::kind() const {
    return ExprKind::CallExpr;
}
void CallExpr::print(ASTPrinter& printer) const {
    NodePrinter node("CallExpr", printer);
    node.field("fn_name", fn_name.name);

    node.name("args");

    {
        ArrayPrinter array(args.size(), printer);
        for(size_t i = 0; i < args.size(); i++) {
            array.print_item(args[i].get());
        }
    }
}
AssignmentExpr::AssignmentExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
}

ExprKind AssignmentExpr::kind() const {
    return ExprKind::AssignmentExpr;
}

void AssignmentExpr::print(ASTPrinter &printer) const {
    NodePrinter node("AssignmentExpr", printer);
    node.field("lhs", lhs.get());
    node.field("lhs", rhs.get());
}
LetExpr::LetExpr(std::unique_ptr<Variable> variable, const std::optional<Identifier>& type, std::optional<std::unique_ptr<Expr>> rhs) {
    this->variable = std::move(variable);
    this->type = type;
    this->rhs = std::move(rhs);
}
ExprKind LetExpr::kind() const {
    return ExprKind::LetExpr;
}
void LetExpr::print(ASTPrinter& printer) const {
    NodePrinter node("LetExpr", printer);
    node.field("variable", variable.get());

    if(type) {
        node.field("type", type->name);
    }

    if(rhs) {
        node.field("rhs", rhs->get());
    }
}

ExprStatement::ExprStatement(std::unique_ptr<Expr> expr) {
    this->expr = std::move(expr);
}

StatementKind ExprStatement::kind() const {
    return StatementKind::Expr;
}

void ExprStatement::print(ASTPrinter& printer) const {
    NodePrinter node("ExprStatement", printer);
    node.field("expr", expr.get());
}
BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>>& statements) {
    this->statements = std::move(statements);
}

StatementKind BlockStatement::kind() const {
    return StatementKind::Block;
}

void BlockStatement::print(ASTPrinter& printer) const {
    NodePrinter node("BlockStatement", printer);
    node.name("expr");
    {
        ArrayPrinter array(statements.size(), printer);
        for(size_t i = 0; i < statements.size(); i++) {
            array.print_item(statements[i].get());
        }
    }
}
FunctionArg::FunctionArg(const Identifier& name, const Identifier& type) {
    this->name = name;
    this->type = type;
}
void FunctionArg::print(ASTPrinter& printer) const {
    printer.print(std::format("{}: {}", name.name, type.name));
}
Function::Function(const Identifier& name, FunctionArgs&& args, std::unique_ptr<BlockStatement> block, const std::optional<Identifier>& return_type) {
    this->name = name;
    this->args = args;
    this->block = std::move(block);
    this->return_type = return_type;
}
StatementKind Function::kind() const {
    return StatementKind::Function;
}
void Function::print(ASTPrinter& printer) const {
    NodePrinter node("Function", printer);
    node.field("name", name.name);
    node.name("args");

    {
        ArrayPrinter array(args.size(), printer);
        for(size_t i = 0; i < args.size(); i++) {
            array.print_item(&args[i]);
        }
    }

    node.name("block");
    {
        ArrayPrinter array(block->statements.size(), printer);
        for(size_t i = 0; i < block->statements.size(); i++) {
            array.print_item(block->statements[i].get());
        }
    }

    if(return_type) {
        node.field("return_type", return_type->name);
    }

}

ReturnStatement::ReturnStatement(std::optional<std::unique_ptr<Expr>> value) {
    this->value = std::move(value);
}
StatementKind ReturnStatement::kind() const {
    return StatementKind::Return;
}
void ReturnStatement::print(ASTPrinter& printer) const {
    if(!value) {
        printer.println("ReturnStatement");
    } else {
        NodePrinter node("ReturnStatement", printer);

        node.field("value", value->get());
    }
}
}