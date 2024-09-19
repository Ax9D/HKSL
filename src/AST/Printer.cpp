#include <AST/Printer.h>
#include <format>
#include <cassert>
#include <iostream>

namespace HKSL {
ASTPrinter::ASTPrinter() {
    indent = 0;
}
void ASTPrinter::increase_depth() {
    indent++;
}
void ASTPrinter::decrease_depth() {
    indent--;
}
void ASTPrinter::print_with_indent(const std::string& text) {
    std::string indent_text(indent, '\t');

    print(indent_text);
    print(text);
}
void ASTPrinter::println_with_indent(const std::string& text) {
    std::string indent_text(indent, '\t');

    print(indent_text);
    println(text);
}
void ASTPrinter::print(const std::string& text) {
    std::cout << text;
}
void ASTPrinter::println(const std::string& text) {
    print(text);

    std::cout << std::endl;
}
void ASTPrinter::println() {

    std::cout << std::endl;
}

NodePrinter::NodePrinter(const std::string& name, ASTPrinter& print): printer(print) {
    printer.println(std::format("{} {{", name));
    printer.increase_depth();
}
NodePrinter::~NodePrinter() {
    printer.decrease_depth();
    printer.print_with_indent("}");
}
NodePrinter& NodePrinter::name(const std::string& name) {
    printer.print_with_indent(std::format("{}: ", name));

    return *this;
}
NodePrinter& NodePrinter::value(const ASTPrint* node) {
    node->print(printer);
    printer.println();

    return *this;
}
NodePrinter& NodePrinter::field(const std::string &name, const std::string &value) {
    printer.println_with_indent(std::format("{}: {}", name, value));

    return *this;
}
NodePrinter& NodePrinter::field(const std::string &name, const ASTPrint* value) {
    printer.print_with_indent(std::format("{}: ", name));
    value->print(printer);
    printer.println();
    return *this;
}

ArrayPrinter::ArrayPrinter(size_t n, ASTPrinter& printer_): printer(printer_)  {
    this->n = n;
    this->i = 0;
    printer.println("[");
    printer.increase_depth();
}
ArrayPrinter::~ArrayPrinter() {
    printer.decrease_depth();
    printer.println_with_indent("]");
}
void ArrayPrinter::print_item(const ASTPrint* item) {
    printer.print_with_indent("");
    item->print(printer);
    
    assert(i < n);

    if(i < n - 1) {
        printer.print(",");
    }
    printer.println();

    i++;
}
}