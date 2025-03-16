#include <Function.h>
namespace HKSL {
LibraryFunction::LibraryFunction(const std::string& name, std::initializer_list<Type*> args, Type* return_type): m_name(name), m_args(args), m_return_type(return_type) {}
const char* LibraryFunction::name() const {
    return m_name.c_str();
}
size_t LibraryFunction::n_args() const {
    return m_args.size();
}
Type* LibraryFunction::arg_at(size_t i) const {
    return m_args[i];
}
Type* LibraryFunction::return_type() const {
    return m_return_type;
}
}