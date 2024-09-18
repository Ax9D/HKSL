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
        NodePrinter& name(const std::string& name);
        NodePrinter& value(const Print* node);
    private:
        Printer& printer;
};


class ArrayPrinter {
    public:
        ArrayPrinter(size_t n, Printer& printer);
        ~ArrayPrinter();
        void print_item(const Print* item);
    private:
        Printer& printer;
        size_t i;
        size_t n;
};
}
