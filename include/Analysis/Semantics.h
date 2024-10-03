#pragma once

#include <Analysis/Visitor.h>
#include <optional>
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
    Span span;
    bool initialized = false;
};
class Scope {
    public:
        Scope(ScopeKind kind);
        bool is_block() const;
        bool is_function() const;
        bool is_global() const;
        VariableData* push_var(const Identifier& name);
        bool var_exists(const std::string& name);
        VariableData* find_var(const std::string& name);
        void for_each_var(std::function<void (const std::string&, const VariableData&)> fn) const;
    private:

    ScopeKind kind;
    std::unordered_map<std::string, VariableData> variables;
};

struct SemanticsAnalysisResult {
    bool is_success();
    std::vector<std::string> errors;
};
class SemanticsVisitor: private Visitor {
    public:
        SemanticsVisitor();
        SemanticsAnalysisResult run(const AST& ast);
    private:
        void visit(const AST& ast) override;
        void visit_function(const Function* func) override;
        void visit_block_statement(const BlockStatement* block) override;
        void visit_let_expr(const LetExpr* expr) override;
        void visit_assignment_expr(const AssignmentExpr* expr) override;

        Scope& current_scope();
        void push_block();
        void pop_block();
        void push_function();
        void pop_function();
        bool var_exists(const std::string& name);
        VariableData* find_var(const std::string& name);
        void check_uninitialized();

        std::vector<Scope> scope_stack;
        std::vector<std::string> errors;
};
}