#include "AST.h"
#include "Util.h"
#include <Visitor.h>

namespace HKSL {
void Visitor::visit(AST& ast) {
    for(auto& statement: ast.statements) {
        visit_statement(statement.get());
    }
}
void Visitor::visit_statement(Statement* statement) {
    switch(statement->kind()) {
        case StatementKind::Expr:
            return visit_expr_statement((ExprStatement*) statement);
        case StatementKind::If:
            return visit_if_statement((IfStatement*) statement);
        case StatementKind::Else:
            return visit_else_statement((ElseStatement*) statement);
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
void Visitor::visit_expr_statement(ExprStatement* expr) {
    visit_expr(expr->expr.get());
}
void Visitor::visit_if_statement(IfStatement* if_statement) {
    visit_expr(if_statement->condition.get());
    visit_block_statement(if_statement->then_block.get());
    if(if_statement->else_stmt) {
        visit_statement(if_statement->else_stmt->get());
    }
}
void Visitor::visit_else_statement(ElseStatement* else_statement) {
    visit_statement(else_statement->statement.get());
}
void Visitor::visit_block_statement(BlockStatement* block) {
    for(auto& statement: block->statements) {
        visit_statement(statement.get());
    }
}
void Visitor::visit_function(Function* function) {
    visit_function_name(function->m_name);

    for(auto arg: function->m_args) {
        visit_function_arg(&arg);
    }

    if(function->m_return_type) {
        visit_type(function->m_return_type);
    }

    visit_block_statement(function->m_block.get());
}
void Visitor::visit_return_statement(ReturnStatement* return_statement) {
    if(return_statement->value) {
        visit_expr(return_statement->value->get());
    }
}
void Visitor::visit_expr(Expr* expr) {
    switch(expr->kind()) {
        case ExprKind::BinExpr:
            return visit_binary_expr((BinExpr*) expr);
        case ExprKind::UnaryExpr:
            return visit_unary_expr((UnaryExpr*) expr);
        case ExprKind::Variable:
            return visit_variable((Variable*) expr);
        case ExprKind::VarDecl:
            return visit_var_decl((VarDecl*) expr);
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
void Visitor::visit_binary_expr(BinExpr* expr) {
    visit_binary_op(expr->op);
    visit_expr(expr->left.get());
    visit_expr(expr->right.get());
}
void Visitor::visit_unary_expr(UnaryExpr* expr) {
    visit_unary_op(expr->op);
    visit_expr(expr->expr.get());
}
void Visitor::visit_variable(Variable* variable) {
    visit_variable_name(variable->name);
}
void Visitor::visit_var_decl(VarDecl* var_decl) {
    visit_variable_name(var_decl->name);

    if(var_decl->type) {
        visit_type(*var_decl->type);
    }
}
void Visitor::visit_number_constant(NumberConstant* expr) {
    
}
void Visitor::visit_call_expr(CallExpr* expr) {
    visit_function_name(expr->fn_name);
    for(const auto& arg: expr->args) {
        visit_call_arg(arg.get());
    }
}
void Visitor::visit_assignment_expr(AssignmentExpr* expr) {
    visit_expr(expr->lhs.get());
    visit_expr(expr->rhs.get());
}
void Visitor::visit_let_expr(LetExpr* expr) {
    visit_var_decl(expr->var_decl.get());

    if(expr->rhs) {
        visit_expr(expr->rhs->get());
    }
}

void Visitor::visit_binary_op(BinOp op) {}
void Visitor::visit_unary_op(UnaryOp op) {}
void Visitor::visit_variable_name(Identifier& name) {
    visit_identifier(name);
}
void Visitor::visit_function_name(Identifier& name) {
    visit_identifier(name);
}
void Visitor::visit_identifier(Identifier& identifier) {}
void Visitor::visit_type(Type* type) {}
void Visitor::visit_function_arg(VarDecl* arg) {
    visit_var_decl(arg);
}
void Visitor::visit_call_arg(Expr* arg) {
    visit_expr(arg);
}
}