#include <TypeCheck.h>
#include <Context.h>
#include <Parser.h>
#include <Semantics.h>
#include <Compiler.h>
namespace HKSL {
bool CompilationResult::is_success() {
    return errors.empty();
}

Compiler::Compiler() {}
CompilationResult Compiler::compile(const std::string& filename, const std::string& source) {
    Lexer lexer(source.c_str());

    auto tokens = lexer.collect_tokens();
    CompilationContext context;

    Parser parser(context, tokens.data());
    auto ast = parser.program();
    
    context.set_ast(std::move(ast));
    SemanticsVisitor semantics_visitor(context);

    semantics_visitor.run();
    context.abort_if_failure();

    TypeInferenceVisitor type_inference_visitor(context);
    type_inference_visitor.run();
    context.abort_if_failure();

    auto result = CompilationResult {
        .errors = std::move(context.errors())
    };

    return result;
}
}