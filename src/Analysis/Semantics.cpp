#include <Analysis/Semantics.h>
#include <__format/format_functions.h>
#include <format>
#include <cassert>

namespace HKSL {

Scope::Scope(ScopeKind _kind): kind(_kind)  {
}
bool Scope::is_block() const {
    return kind == ScopeKind::Block;
}
bool Scope::is_function() const {
    return kind == ScopeKind::Function;
}
bool Scope::is_global() const {
    return kind == ScopeKind::Global;
}
VariableData* Scope::push_var(const Identifier &name) {
    variables[name.name] = VariableData {
        .span = name.span,
        .initialized = false
    };

    return &variables[name.name];
}
bool Scope::var_exists(const std::string& name) {
    return find_var(name) != nullptr;
}
VariableData* Scope::find_var(const std::string& name) {
    auto it = variables.find(name);
    if(it == variables.end()) {
        return nullptr;
    }

    return &it->second;
}
void Scope::for_each_var(std::function<void (const std::string &, const VariableData &)> fn) const {
    for(const auto& [name, data]: variables) {
        fn(name, data);
    }
}
SemanticsAnalysisResult SemanticsVisitor::run(const AST& ast) {
    visit(ast);
    check_uninitialized();

    return SemanticsAnalysisResult {
        .errors = std::move(errors)
    };
}
SemanticsVisitor::SemanticsVisitor() {
    scope_stack.push_back(Scope(ScopeKind::Global));
}
void SemanticsVisitor::visit(const AST &ast) {
    Visitor::visit(ast);
}
void SemanticsVisitor::visit_function(const Function* function) {
    push_function();
    Visitor::visit_function(function);
    pop_function();
}
void SemanticsVisitor::visit_block_statement(const BlockStatement* block) {
    push_block();
    Visitor::visit_block_statement(block);
    pop_block();
}
void SemanticsVisitor::visit_let_expr(const LetExpr* expr) {
    auto& var = expr->variable->name;
    // Variable names must be unique in a scope; no shadowing
    if(current_scope().var_exists(var.name)) {
        Span current_span = var.span;
        errors.push_back(std::format("Redefinition of {} on {}:{}", var.name, current_span.line, current_span.col));
        return;
    } else {
        auto* new_var = current_scope().push_var(var);
        new_var->initialized = true;
    }
}
void SemanticsVisitor::visit_assignment_expr(const AssignmentExpr* expr) {
    assert(expr->lhs->kind() == ExprKind::Variable);
    auto variable = (const Variable*)(expr->lhs.get());

    auto* prev_var = find_var(variable->name.name);
    if(!prev_var) {
        Span current_span = variable->name.span;
        errors.push_back(std::format("Use of undeclared variable {} on {}:{}", variable->name.name, current_span.line, current_span.col));
        return;
    }

    prev_var->initialized = true;
}
Scope& SemanticsVisitor::current_scope() {
    return scope_stack.back();
}
void SemanticsVisitor::push_block() {
    scope_stack.push_back(Scope(ScopeKind::Block));
}
void SemanticsVisitor::pop_block() {
    assert(scope_stack.back().is_block());
    scope_stack.pop_back();
}
void SemanticsVisitor::push_function() {
    scope_stack.push_back(Scope(ScopeKind::Function));
}
void SemanticsVisitor::pop_function() {
    assert(scope_stack.back().is_function());
    scope_stack.pop_back();
}
bool SemanticsVisitor::var_exists(const std::string& name) {
    return find_var(name) != nullptr;
}
VariableData* SemanticsVisitor::find_var(const std::string& name) {
    int64_t index = scope_stack.size() - 1;
    while(index >= 1 /*Global scope is at index 0*/) {
        auto& top = scope_stack[index];
        if(top.is_function()) {
            break;
        }

        auto* matching_var = top.find_var(name);

        if(matching_var) {
            return matching_var;
        }

        index--;
    }

    return nullptr;
}
void SemanticsVisitor::check_uninitialized() {
    for(const auto& scope: scope_stack) {
        scope.for_each_var([this](const std::string& name, const VariableData& data){
            if(!data.initialized) {
                errors.push_back(std::format("Variable {} has not been initialized {}:{}", name, data.span.line, data.span.col));
            }
        });
    }
}
bool SemanticsAnalysisResult::is_success() {
    return errors.empty();
}
}