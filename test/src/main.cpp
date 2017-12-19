#include <x86_64_compiler/compiler.hpp>

#include <iostream>

int main()
{
    x86_64::Compiler c;

    c.constant(int8_t{100});
    c.constant(200);
    c.constant(3.14);

    std::cout << c.getCode() << std::endl;

    return 0;
}
