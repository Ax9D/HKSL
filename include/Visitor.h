#pragma once
#include "Util.h"
#include <AST.h>

namespace HKSL {
class Visitor {
    public:
        ~Visitor() = default;
        virtual void visit(const AST& ast);
        virtual void visit_statement(const Statement* statement);
        virtual void visit_expr_statement(const ExprStatement* expr);
        virtual void visit_if_statement(const IfStatement* if_statement);
        virtual void visit_block_statement(const BlockStatement* block);
        virtual void visit_function(const Function* function);
        virtual void visit_return_statement(const ReturnStatement* return_statement);
        virtual void visit_expr(const Expr* expr);
        virtual void visit_binary_expr(const BinExpr* expr);
        virtual void visit_unary_expr(const UnaryExpr* expr);
        virtual void visit_variable(const Variable* variable);
        virtual void visit_number_constant(const NumberConstant* expr);
        virtual void visit_call_expr(const CallExpr* expr);
        virtual void visit_assignment_expr(const AssignmentExpr* expr);
        virtual void visit_let_expr(const LetExpr* expr);

        virtual void visit_binary_op(BinOp op);
        virtual void visit_unary_op(UnaryOp op);
        virtual void visit_variable_name(const Identifier& name);
        virtual void visit_function_name(const Identifier& name);
        virtual void visit_identifier(const Identifier& identifier);
        virtual void visit_type(const Identifier& type);
        virtual void visit_function_arg(const FunctionArg& arg);
        virtual void visit_call_arg(const Expr* arg);
};

}