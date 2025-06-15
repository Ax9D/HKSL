#pragma once
#include <Visitor.h>
#include <Context.h>
#include <Typing.h>

namespace HKSL {
// class TypeExistsVisitor: public Visitor {
//     public:
//         TypeExistsVisitor(CompilationContext& context);
//         bool check_all_types_exists();
//     private:
//         void visit_type(Identifier&) override;
//         CompilationContext& context;
// };
class TypeInferenceVisitor: public Visitor {
    public:
        TypeInferenceVisitor(CompilationContext& context);
        bool run();
    private:
        void visit_let_expr(LetExpr* expr) override;
        void visit_expr(Expr* expr) override;
        void visit_function(Function* function) override;
        void visit_return_statement(ReturnStatement* ret) override;
        Type* type_of(const Expr* expr);
        Type* type_of_expr(const Expr* expr);
        Type* type_of_variable(const Variable* var);
        Type* type_of_let_expr(const LetExpr* expr);
        Type* type_of_assignment_expr(const AssignmentExpr* expr);
        Type* type_of_call_expr(const CallExpr* expr);
        Type* type_of_var_decl(const VarDecl* decl);
        Type* type_of_unary_expr(const UnaryExpr* expr);
        Type* type_of_binary_expr(const BinExpr* expr);
        Type* type_of_number_constant(const NumberConstant* expr);
        CompilationContext& context;
        std::optional<Function*> outer_fn;
};
}