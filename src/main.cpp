#include "FSUtil.h"
#include <Lexer.h>
#include <Parser.h>

struct CLIArgs {
    const char* src_path;
    void parse(int argc, const char** argv) {
        if(argc != 2) {
            HKSL_ERROR("Please provide a source file path");
        }

        src_path = argv[1];
    }
};
int main(int argc, const char** argv) {
    CLIArgs args;
    args.parse(argc, argv);
    std::string code = HKSL::read_to_string(args.src_path);
    std::cout << code << std::endl;

    HKSL::Lexer lexer(code.c_str());
    std::vector<HKSL::Token> tokens = lexer.collect_tokens();
    HKSL::Parser parser(tokens.data());

    auto statements = parser.statements();

    HKSL::Printer printer;
    for(auto& statement: statements) {
        statement->print(printer);
        printer.println();
    }
}