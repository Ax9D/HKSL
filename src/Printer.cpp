#include <AST.h>
#include <Printer.h>
#include <format>

namespace HKSL {
Printer::Printer() {
    indent = 0;
}
void Printer::increase_depth() {
    indent++;
}
void Printer::decrease_depth() {
    indent--;
}
void Printer::print_with_indent(const std::string& text) {
    std::string indent_text(indent, '\t');

    print(indent_text);
    print(text);
}
void Printer::println_with_indent(const std::string& text) {
    std::string indent_text(indent, '\t');

    print(indent_text);
    println(text);
}
void Printer::print(const std::string& text) {
    std::cout << text;
}
void Printer::println(const std::string& text) {
    print(text);

    std::cout << std::endl;
}
void Printer::println() {

    std::cout << std::endl;
}

NodePrinter::NodePrinter(const std::string& name, Printer& print): printer(print) {
    printer.println(std::format("{} {{", name));
    printer.increase_depth();
}
NodePrinter::~NodePrinter() {
    printer.decrease_depth();
    printer.print_with_indent("}");
}
NodePrinter& NodePrinter::field(const std::string &name, const std::string &value) {
    printer.println_with_indent(std::format("{}: {}", name, value));

    return *this;
}
NodePrinter& NodePrinter::field(const std::string &name, const Print* value) {
    printer.print_with_indent(std::format("{}: ", name));
    value->print(printer);
    printer.println();
    return *this;
}
}