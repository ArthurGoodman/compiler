#include <iostream>

#include "compiler.h"

int main() {
    x86::Compiler c;

    // c.rdata("string", "Hello, World!");

    // c.externalFunction("puts");

    // c.function("f");

    // c.push(x86::EBP);
    // c.mov(x86::ESP, x86::EBP);
    // c.push(c.abs("string"));
    // c.call(c.rel("puts"));
    // c.leave();
    // c.ret();

    // c.writeOBJ().write("a.o");

    // system("g++ -c -o test.o test.cpp");
    // system("g++ -o test.exe test.o a.o");
    // system("test.exe");

    // system("objdump -x a.o");
    // system("objdump -d a.o");

    const char *str = "Hello, World!";

    c.push(x86::EBP);
    c.push(x86::EBX);
    c.mov(x86::ESP, x86::EBP);
    c.push(reinterpret_cast<int>(str));
    c.mov(reinterpret_cast<int>(puts), x86::EBX);
    c.call(x86::EBX);
    c.mov(0, x86::EAX);
    c.mov(x86::EBP, x86::ESP);
    c.pop(x86::EBX);
    c.pop(x86::EBP);
    c.ret();

    x86::Function f = c.compileFunction();
    std::cout << f.dump() << "\n";
    std::cout << f.invoke() << "\n";

    return 0;
}
