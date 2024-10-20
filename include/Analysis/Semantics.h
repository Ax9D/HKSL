#pragma once

#include "AST.h"
#include <Analysis/Visitor.h>
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

class SymbolResolver {
    public: 
        SymbolResolver();
        Function* get_function(const CallExpr* expr);
        Variable* get_var_decl(const Variable* var);
    private:
        std::unordered_map<const Variable*, const VarDecl*> ref_to_decl;
        std::unordered_map<const CallExpr*, const Function*> call_to_func_decl;
};

struct SemanticsAnalysisResult {
    bool is_success();
    std::vector<std::string> errors;
    std::unordered_map<const Variable*, const VarDecl*> ref_to_decl;
};
class SemanticsVisitor: private Visitor {
    public:
        SemanticsVisitor();
        SemanticsAnalysisResult run(const AST& ast);
    private:
        void visit(const AST& ast) override;
        void visit_function(const Function* func) override;
        void visit_block_statement(const BlockStatement* block) override;
        void visit_var_decl(const VarDecl* var_decl) override;
        void visit_let_expr(const LetExpr* let_expr) override;
        void visit_call_expr(const CallExpr* call_expr) override;
        void visit_assignment_expr(const AssignmentExpr* assignment_expr) override;
        void visit_variable(const Variable* variable) override;
        
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

        std::vector<Scope> scope_stack;
        std::vector<std::string> errors;
        std::unordered_map<const Variable*, const VarDecl*> ref_to_decl;
};
}