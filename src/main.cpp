#include "Compiler.h"
#include "FSUtil.h"

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

    HKSL::Compiler compiler;
    auto result = compiler.compile(args.src_path, code);

    if(!result.is_success()) {
        for(auto error: result.errors) {
            std::cout << error << std::endl;
            std::exit(-1);
        }    
    }
}