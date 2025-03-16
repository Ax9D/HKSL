#pragma once
#include <AST.h>
#include <unordered_map>
#include <Typing.h>

namespace HKSL {

class SymbolResolver {
    public: 
        SymbolResolver() = default;
        // void register_function(const FunctionDef* function);
        void register_variable_ref(const Variable* variable, const VarDecl* decl);
        void register_function_ref(const CallExpr* call_expr, const Function* decl);

        const Function* get_function(const CallExpr* expr);
        const VarDecl* get_var_decl(const Variable* var);
    private:
        std::unordered_map<const Variable*, const VarDecl*> ref_to_decl;
        std::unordered_map<const CallExpr*, const Function*> call_to_func_decl;
};

class CompilationContext {
    public:
        CompilationContext();
        void set_ast(std::unique_ptr<AST> ast);
        AST& get_ast();
        SymbolResolver& symbol_resolver();
        TypeRegistry& type_registry();
        TypeResolver& type_resolver();
        void error(Span location, const std::string& message);
        const std::vector<std::string>& errors();
        void print_errors();
        bool is_success();
        void abort_if_failure();
    private:
        bool is_failing;
        SymbolResolver sym_resolver;
        TypeRegistry ty_registry;
        TypeResolver ty_resolver;
        std::vector<std::string> m_errors;
        std::unique_ptr<AST> ast;
};
}