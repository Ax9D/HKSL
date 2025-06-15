#pragma once

#include <AST.h>
#include <Context.h>
#include <Visitor.h>
#include <vector>
#include <unordered_map>
#include <functional>

namespace HKSL {

enum class ScopeKind {
    Block,
    Function,
    Global
};

struct VariableData {
    const VarDecl* decl;
    Span span;
    bool initialized = false;
};

class Scope {
    public:
        Scope(ScopeKind kind);
        bool is_block() const;
        bool is_function() const;
        bool is_global() const;
        VariableData* push_var_decl(const VarDecl* var);
        void push_function(const Function* func);
        bool var_exists(const std::string& name);
        bool func_exists(const std::string& name);
        VariableData* find_var_decl(const std::string& name);
        const Function* find_func_decl(const std::string& name);
        void for_each_var(std::function<void (const std::string&, const VariableData&)> fn) const;
    private:

    ScopeKind kind;
    std::unordered_map<std::string, VariableData> variables;
    std::unordered_map<std::string, const Function*> functions; 
};

class SemanticsVisitor: private Visitor {
    public:
        SemanticsVisitor(CompilationContext& context);
        bool run();
    private:
        void visit(AST& ast) override;
        void visit_function(Function* func) override;
        void visit_block_statement(BlockStatement* block) override;
        void visit_var_decl(VarDecl* var_decl) override;
        void visit_let_expr(LetExpr* let_expr) override;
        void visit_call_expr(CallExpr* call_expr) override;
        void visit_assignment_expr(AssignmentExpr* assignment_expr) override;
        void visit_variable(Variable* variable) override;
        
        VariableData* check_variable(const Variable* variable);
        
        Scope& current_scope();
        void push_block();
        void pop_block();
        void push_function(const Function* function);
        void pop_function();
        bool var_exists(const std::string& name);
        bool func_exists(const std::string& name);
        VariableData* find_var_decl(const std::string& name);
        const Function* find_func_decl(const std::string& name);
        void check_uninitialized();

        CompilationContext& context;
        std::vector<Scope> scope_stack;
};
}