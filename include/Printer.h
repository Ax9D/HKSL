#pragma once 
#include <AST.h>

namespace HKSL {

class Printer;

struct Print {
    ~Print() = default;
    virtual void print(Printer& printer) const = 0;
};

class Printer {
    public:
        Printer();
        void print(const std::string& text);
        void println(const std::string& text);
        void println();
        void print_with_indent(const std::string& text);
        void println_with_indent(const std::string& text);
        void increase_depth();
        void decrease_depth();
    private:
        uint32_t indent;
};
class NodePrinter {
    public:
        NodePrinter(const std::string& name, Printer& printer);
        ~NodePrinter();
        NodePrinter& field(const std::string& name, const std::string& value);
        NodePrinter& field(const std::string& name, const Print* node);
    private:
        Printer& printer;
};
}