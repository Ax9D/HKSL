#pragma once 
#include <string>

namespace HKSL {

class ASTPrinter;

struct ASTPrint {
    ~ASTPrint() = default;
    virtual void print(ASTPrinter& printer) const = 0;
};

class ASTPrinter {
    public:
        ASTPrinter();
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
        NodePrinter(const std::string& name, ASTPrinter& printer);
        ~NodePrinter();
        NodePrinter& field(const std::string& name, const std::string& value);
        NodePrinter& field(const std::string& name, const ASTPrint* node);
        NodePrinter& name(const std::string& name);
        NodePrinter& value(const ASTPrint* node);
    private:
        ASTPrinter& printer;
};


class ArrayPrinter {
    public:
        ArrayPrinter(size_t n, ASTPrinter& printer);
        ~ArrayPrinter();
        void print_item(const ASTPrint* item);
    private:
        ASTPrinter& printer;
        size_t i;
        size_t n;
};
}
