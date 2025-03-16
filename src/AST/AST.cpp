#include <AST/AST.h>
#include <format>
namespace HKSL {
bool expr_kind_is_place(ExprKind kind) {
    return kind == ExprKind::Variable || kind == ExprKind::CallExpr;
}
const char* expr_kind_to_string(ExprKind kind) {
    switch(kind) {
        case ExprKind::BinExpr:
            return "BinExpr";
        case ExprKind::UnaryExpr:
            return "UnaryExpr";
        case ExprKind::NumberConstant:
            return "NumberConstant";
        case ExprKind::Variable:
            return "Variable";
        case ExprKind::VarDecl:
            return "VarDecl";
        case ExprKind::CallExpr:
            return "CallExpr";
        case ExprKind::AssignmentExpr:
            return "AssignmentExpr";
        case ExprKind::LetExpr:
            return "LetExpr";
    }
}
std::string unary_op_to_string(UnaryOp op) {
    switch(op) {
        case UnaryOp::Negate:
            return "-";
        default:
            HKSL_ERROR("Unimplemented");
    }
}
UnaryExpr::UnaryExpr(UnaryOp op, std::unique_ptr<Expr> expr, Token op_token) {
    this->op = op;
    this->expr = std::move(expr);
    this->op_token = op_token;
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
BinExpr::BinExpr(BinOp op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right, Token op_token) {
    this->op = op;
    this->left = std::move(left);
    this->right = std::move(right);
    this->op_token = op_token;
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

VarDecl::VarDecl(const Identifier& name) {
    this->name = name;
    this->type = std::nullopt;
}
VarDecl::VarDecl(const Identifier& name, std::optional<Type*>& type) {
    this->name = name;
    this->type = type;
}
ExprKind VarDecl::kind() const {
    return ExprKind::VarDecl;
}
void VarDecl::print(ASTPrinter& printer) const {
    printer.print(std::format("VarDecl({})", name.name));
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
AssignmentExpr::AssignmentExpr(std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs, Token eq_token) {
    this->lhs = std::move(lhs);
    this->rhs = std::move(rhs);
    this->eq_token = eq_token;
}

ExprKind AssignmentExpr::kind() const {
    return ExprKind::AssignmentExpr;
}

void AssignmentExpr::print(ASTPrinter &printer) const {
    NodePrinter node("AssignmentExpr", printer);
    node.field("lhs", lhs.get());
    node.field("lhs", rhs.get());
}
LetExpr::LetExpr(std::unique_ptr<VarDecl> var_decl, const std::optional<Identifier>& type, std::optional<std::unique_ptr<Expr>> rhs) {
    this->var_decl = std::move(var_decl);
    this->rhs = std::move(rhs);
}
ExprKind LetExpr::kind() const {
    return ExprKind::LetExpr;
}
void LetExpr::print(ASTPrinter& printer) const {
    NodePrinter node("LetExpr", printer);
    node.field("var_decl", var_decl.get());

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
    node.name("statements");
    {
        ArrayPrinter array(statements.size(), printer);
        for(size_t i = 0; i < statements.size(); i++) {
            array.print_item(statements[i].get());
        }
    }
}
// FunctionArg::FunctionArg(std::unique_ptr<Variable> variable, const Identifier& _type): type(_type){
//     this->variable = std::move(variable);
// }
// void FunctionArg::print(ASTPrinter& printer) const {
//     printer.print(std::format("{}: {}", variable->name.name, type.name));
// }
Function::Function(const Identifier& name, FunctionArgs& args, std::unique_ptr<BlockStatement> block, Type* return_type) {
    this->m_name = name;
    this->m_args = std::move(args);
    this->m_block = std::move(block);
    this->m_return_type = return_type;
}
StatementKind Function::kind() const {
    return StatementKind::Function;
}
void Function::print(ASTPrinter& printer) const {
    NodePrinter node("Function", printer);
    node.field("name", m_name.name);
    node.name("args");

    {
        ArrayPrinter array(m_args.size(), printer);
        for(size_t i = 0; i < m_args.size(); i++) {
            array.print_item(&m_args[i]);
        }
    }

    node.name("block");
    {
        ArrayPrinter array(m_block->statements.size(), printer);
        for(size_t i = 0; i < m_block->statements.size(); i++) {
            array.print_item(m_block->statements[i].get());
        }
    }

    node.field("return_type", m_return_type->name());
}

ReturnStatement::ReturnStatement(std::optional<std::unique_ptr<Expr>> value, Token ret_token) {
    this->value = std::move(value);
    this->ret_token = ret_token;
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

IfStatement::IfStatement(std::unique_ptr<Expr> condition, std::unique_ptr<BlockStatement> block, std::optional<std::unique_ptr<ElseStatement>> else_stmt) {
    this->condition = std::move(condition);
    this->then_block = std::move(block);
    this->else_stmt = std::move(else_stmt);
}
StatementKind IfStatement::kind() const {
    return StatementKind::If;
}
void IfStatement::print(ASTPrinter &printer) const {
    NodePrinter node("IfStatement", printer);
    node.field("condition", condition.get());
    node.field("condition", then_block.get());

    if(else_stmt) {
        node.field("else", else_stmt->get());
    }
}
ElseStatement::ElseStatement(std::unique_ptr<Statement> statement) {
    this->statement = std::move(statement);
}
StatementKind ElseStatement::kind() const {
    return StatementKind::Else;
}
void ElseStatement::print(ASTPrinter& printer) const {
    NodePrinter node("ElseStatement", printer);
    node.field("statement", statement.get());
}
AST::AST(std::vector<std::unique_ptr<Statement>>& statements) {
    this->statements = std::move(statements);
}
void AST::print(ASTPrinter &printer) const {
    ArrayPrinter array(statements.size(), printer);

    for(size_t i = 0; i < statements.size(); i++) {
        array.print_item(statements[i].get());
    }
}
}