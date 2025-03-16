#pragma once
#include <vector>
#include <Typing.h>
#include <string>

namespace HKSL {
class FunctionDef {
    public:
        virtual const char* name() const = 0;
        virtual Type* arg_at(size_t i) const = 0;
        virtual size_t n_args() const = 0;
        virtual Type* return_type() const = 0;
};

class LibraryFunction: public FunctionDef {
    public: 
        LibraryFunction(const std::string& name, std::initializer_list<Type*> args, Type* return_type);
        const char* name() const override;
        Type* arg_at(size_t i) const override;
        size_t n_args() const override;
        Type* return_type() const override;
    private:
        std::string m_name;
        std::vector<Type*> m_args;
        Type* m_return_type;
};
}