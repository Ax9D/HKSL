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
VariableData* Scope::push_var_decl(const VarDecl* var) {
    variables[var->name.name] = VariableData {
        .decl = var,
        .span = var->name.span,
        .initialized = false,
    };

    return &variables[var->name.name];
}
void Scope::push_function(const Function* func) {
    functions[func->m_name.name] = func;
}
bool Scope::var_exists(const std::string& name) {
    return find_var_decl(name) != nullptr;
}
VariableData* Scope::find_var_decl(const std::string& name) {
    auto it = variables.find(name);
    if(it == variables.end()) {
        return nullptr;
    }

    return &it->second;
}
const Function* Scope::find_func_decl(const std::string& name) {
    auto it = functions.find(name);
    if(it == functions.end()) {
        return nullptr;
    }

    return it->second;
}
void Scope::for_each_var(std::function<void (const std::string &, const VariableData &)> fn) const {
    for(const auto& [name, data]: variables) {
        fn(name, data);
    }
}
bool SemanticsVisitor::run() {
    visit(context.get_ast());
    return context.is_success();
}
SemanticsVisitor::SemanticsVisitor(CompilationContext& _context): context(_context) {
    scope_stack.push_back(Scope(ScopeKind::Global));
}
void SemanticsVisitor::visit(AST& ast) {
    Visitor::visit(ast);
    // Check for unitialized variables in the global scope
    check_uninitialized();
}
void SemanticsVisitor::visit_function(Function* function) {
    if(current_scope().find_func_decl(function->m_name.name)) {
        context.error(function->m_name.span, std::format("Redefinition of function {}", function->m_name.span.to_string()));
        return;
    }
    push_function(function);
    for(const auto& decl: function->m_args) {
        auto var = current_scope().push_var_decl(&decl);
        var->initialized = true;
    }
    Visitor::visit_block_statement(function->m_block.get());
    if(function->m_return_type) {
        Visitor::visit_type(function->m_return_type);
    }
    pop_function();
}
void SemanticsVisitor::visit_call_expr(CallExpr* call_expr) {
    auto function_decl = find_func_decl(call_expr->fn_name.name);
    if(!function_decl) {
        Span current_span = call_expr->fn_name.span;

        context.error(current_span, std::format("Use of undeclared function {}", call_expr->fn_name.name));
        return;
    }

    context.symbol_resolver().register_function_ref(call_expr, function_decl);

    Visitor::visit_call_expr(call_expr);
}
void SemanticsVisitor::visit_block_statement(BlockStatement* block) {
    push_block();
    Visitor::visit_block_statement(block);
    pop_block();
}
void SemanticsVisitor::visit_var_decl(VarDecl* var_decl) {
    auto& var = var_decl->name;
    // Variable names must be unique in a scope; no shadowing
    if(find_var_decl(var.name)) {
        Span current_span = var.span;
        context.error(current_span, std::format("Redefinition of {}", var.name));
        return;
    } else {
        current_scope().push_var_decl(var_decl);
    }

    Visitor::visit_var_decl(var_decl);
}
void SemanticsVisitor::visit_let_expr(LetExpr* let_expr) {
    Visitor::visit_let_expr(let_expr);

    auto decl = find_var_decl(let_expr->var_decl->name.name);
    
    if(decl && let_expr->rhs) {
        decl->initialized = true;
    }
}
void SemanticsVisitor::visit_assignment_expr(AssignmentExpr* assignment_expr) {
    assert(assignment_expr->lhs->kind() == ExprKind::Variable);

    const Variable* assignment_target = (Variable*) assignment_expr->lhs.get();

    auto variable_data = check_variable(assignment_target);

    if(variable_data) {
        variable_data->initialized = true;
    }

    visit_expr(assignment_expr->rhs.get());
}
void SemanticsVisitor::visit_variable(Variable* variable) {
    check_variable(variable);
    Visitor::visit_variable(variable);
}
VariableData* SemanticsVisitor::check_variable(const Variable* variable) {
    auto* prev_var = find_var_decl(variable->name.name);
    if(!prev_var) {
        Span current_span = variable->name.span;
        context.error(current_span, std::format("Use of undeclared variable {}", variable->name.name));
        return nullptr;
    }

    context.symbol_resolver().register_variable_ref(variable, prev_var->decl);
    return prev_var;
}
Scope& SemanticsVisitor::current_scope() {
    return scope_stack.back();
}
void SemanticsVisitor::push_block() {
    scope_stack.push_back(Scope(ScopeKind::Block));
}
void SemanticsVisitor::pop_block() {
    check_uninitialized();

    assert(scope_stack.back().is_block());
    scope_stack.pop_back();

}
void SemanticsVisitor::push_function(const Function* function) {
    current_scope().push_function(function);
    scope_stack.push_back(Scope(ScopeKind::Function));
}
void SemanticsVisitor::pop_function() {
    check_uninitialized();

    assert(scope_stack.back().is_function());
    scope_stack.pop_back();
}
bool SemanticsVisitor::var_exists(const std::string& name) {
    return find_var_decl(name) != nullptr;
}
VariableData* SemanticsVisitor::find_var_decl(const std::string& name) {
    int64_t index = scope_stack.size() - 1;
    while(index >= 1 /*Global scope is at index 0*/) {
        auto& top = scope_stack[index];

        auto* matching_var = top.find_var_decl(name);

        if(matching_var) {
            return matching_var;
        }

        if(top.is_function()) {
            break;
        }

        index--;
    }

    return nullptr;
}
const Function* SemanticsVisitor::find_func_decl(const std::string& name) {
    int64_t index = scope_stack.size() - 1;
    while(index >= 0) {
        auto& top = scope_stack[index];

        auto matching_func = top.find_func_decl(name);

        if(matching_func) {
            return matching_func;
        }

        index--;
    }

    return nullptr;
}
void SemanticsVisitor::check_uninitialized() {
    current_scope().for_each_var([this](const std::string& name, const VariableData& data){
            if(!data.initialized) {
                context.error(data.span, std::format("Variable {} has not been initialized", name));
            }
    });
}
}