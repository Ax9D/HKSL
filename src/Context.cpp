#include <Context.h>
#include <format>
#include <cassert>

namespace HKSL {
CompilationContext::CompilationContext() {
    this->ast = nullptr;
}

void CompilationContext::error(Span location, const std::string &message) {
    is_failing = true;
    m_errors.push_back(std::format("{}: {}", location.to_string(), message));
}
const std::vector<std::string>& CompilationContext::errors() {
    return m_errors;
}
void CompilationContext::set_ast(std::unique_ptr<AST> ast) {
    assert(!this->ast && "AST can only be set once");
    this->ast = std::move(ast);
}
AST& CompilationContext::get_ast() {
    return *ast;
}
SymbolResolver& CompilationContext::symbol_resolver() {
    return sym_resolver;
}
TypeRegistry& CompilationContext::type_registry() {
    return ty_registry;
}
TypeResolver& CompilationContext::type_resolver() {
    return ty_resolver;
}
bool CompilationContext::is_success() {
    return !is_failing;
}
void CompilationContext::print_errors() {
    for(auto error: m_errors) {
        std::cout << error << std::endl;
    }
}
void CompilationContext::abort_if_failure() {
    if(!is_success()) {
        print_errors();
        std::exit(-1);
    }
}

void SymbolResolver::register_variable_ref(const Variable* variable, const VarDecl* decl) {
    ref_to_decl[variable] = decl;
}
void SymbolResolver::register_function_ref(const CallExpr* call_expr, const Function* decl) {
    call_to_func_decl[call_expr] = decl;
}

const Function* SymbolResolver::get_function(const CallExpr* expr) {
    auto it = call_to_func_decl.find(expr);
    if(it != call_to_func_decl.end()) {
        return it->second;
    }

    return nullptr;
}
const VarDecl* SymbolResolver::get_var_decl(const Variable* var) {
    auto it = ref_to_decl.find(var);
    if(it != ref_to_decl.end()) {
        return it->second;
    }

    return nullptr;
}
}
