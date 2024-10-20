
#pragma once
#include <Context.h>

namespace HKSL {
using Errors = std::vector<std::string>;
struct CompilationResult {
    bool is_success();
    Errors errors;
};
class Compiler {
    public:
        Compiler();
        CompilationResult compile(const std::string& filename, const std::string& source);
    private:
};
}