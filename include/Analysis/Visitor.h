#pragma once
#include <Util.h>
#include <AST/AST.h>

namespace HKSL {
class Visitor {
    public:
        ~Visitor() = default;
        virtual void visit(AST& ast);
        virtual void visit_statement(Statement* statement);
        virtual void visit_expr_statement(ExprStatement* expr);
        virtual void visit_if_statement(IfStatement* if_statement);
        virtual void visit_else_statement(ElseStatement* if_statement);
        virtual void visit_block_statement(BlockStatement* block);
        virtual void visit_function(Function* function);
        virtual void visit_return_statement(ReturnStatement* return_statement);
        virtual void visit_expr(Expr* expr);
        virtual void visit_binary_expr(BinExpr* expr);
        virtual void visit_unary_expr(UnaryExpr* expr);
        virtual void visit_variable(Variable* variable);
        virtual void visit_var_decl(VarDecl* var_decl);
        virtual void visit_number_constant(NumberConstant* expr);
        virtual void visit_call_expr(CallExpr* expr);
        virtual void visit_assignment_expr(AssignmentExpr* expr);
        virtual void visit_let_expr(LetExpr* expr);

        virtual void visit_binary_op(BinOp op);
        virtual void visit_unary_op(UnaryOp op);
        virtual void visit_variable_name(Identifier& name);
        virtual void visit_function_name(Identifier& name);
        virtual void visit_identifier(Identifier& identifier);
        virtual void visit_type(Type* type);
        virtual void visit_function_arg(VarDecl* arg);
        virtual void visit_call_arg(Expr* arg);
};
}