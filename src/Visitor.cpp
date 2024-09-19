#include <Visitor.h>

namespace HKSL {
void Visitor::visit(const AST& ast) {
    for(auto& statement: ast.statements) {
        visit_statement(statement.get());
    }
}
void Visitor::visit_statement(const Statement* statement) {
    switch(statement->kind()) {
        case StatementKind::Expr:
            return visit_expr_statement((ExprStatement*) statement);
        case StatementKind::If:
            return visit_if_statement((IfStatement*) statement);
        case StatementKind::Block:
            return visit_block_statement((BlockStatement*) statement);
        case StatementKind::Function:
            return visit_function((Function*) statement);
        case StatementKind::Return:
            return visit_return_statement((ReturnStatement*) statement);
        default:
            HKSL_TODO();
    }
}
void Visitor::visit_expr_statement(const ExprStatement* expr) {
    visit_expr(expr->expr.get());
}
void Visitor::visit_if_statement(const IfStatement* if_statement) {
    HKSL_TODO();
}
void Visitor::visit_block_statement(const BlockStatement* block) {
    for(const auto& statement: block->statements) {
        visit_statement(statement.get());
    }
}
void Visitor::visit_function(const Function* function) {
    visit_function_name(function->name);

    for(const auto& arg: function->args) {
        visit_function_arg(arg);
    }

    if(function->return_type) {
        visit_type(*function->return_type);
    }

    visit_block_statement(function->block.get());
}
void Visitor::visit_return_statement(const ReturnStatement* return_statement) {
    if(return_statement->value) {
        visit_expr(return_statement->value->get());
    }
}
void Visitor::visit_expr(const Expr* expr) {
    switch(expr->kind()) {
        case ExprKind::BinExpr:
            return visit_binary_expr((BinExpr*) expr);
        case ExprKind::UnaryExpr:
            return visit_unary_expr((UnaryExpr*) expr);
        case ExprKind::Variable:
            return visit_variable((Variable*) expr);
        case ExprKind::NumberConstant:
            return visit_number_constant((NumberConstant*) expr);
        case ExprKind::CallExpr:
            return visit_call_expr((CallExpr*) expr);
        case ExprKind::AssignmentExpr:
            return visit_assignment_expr((AssignmentExpr*) expr);
        case ExprKind::LetExpr:
            return visit_let_expr((LetExpr*) expr);
    }
}
void Visitor::visit_binary_expr(const BinExpr* expr) {
    visit_binary_op(expr->op);
    visit_expr(expr->left.get());
    visit_expr(expr->right.get());
}
void Visitor::visit_unary_expr(const UnaryExpr* expr) {
    visit_unary_op(expr->op);
    visit_expr(expr->expr.get());
}
void Visitor::visit_variable(const Variable* variable) {
    visit_variable_name(variable->name);
}
void Visitor::visit_number_constant(const NumberConstant* expr) {
    
}
void Visitor::visit_call_expr(const CallExpr* expr) {
    visit_function_name(expr->fn_name);
    for(const auto& arg: expr->args) {
        visit_call_arg(arg.get());
    }
}
void Visitor::visit_assignment_expr(const AssignmentExpr* expr) {
    visit_expr(expr->lhs.get());
    visit_expr(expr->rhs.get());
}
void Visitor::visit_let_expr(const LetExpr* expr) {
    visit_variable(expr->variable.get());

    if(expr->type) {
        visit_type(*expr->type);
    }
    if(expr->rhs) {
        visit_expr(expr->rhs->get());
    }
}

void Visitor::visit_binary_op(BinOp op) {}
void Visitor::visit_unary_op(UnaryOp op) {}
void Visitor::visit_variable_name(const Identifier& name) {
    visit_identifier(name);
}
void Visitor::visit_function_name(const Identifier& name) {
    visit_identifier(name);
}
void Visitor::visit_identifier(const Identifier& identifier) {}
void Visitor::visit_type(const Identifier& type) {
    visit_identifier(type);
}
void Visitor::visit_function_arg(const FunctionArg& arg) {
    visit_variable_name(arg.name);
    visit_type(arg.type);
}
void Visitor::visit_call_arg(const Expr* arg) {
    visit_expr(arg);
}
}