#include <iostream>

#include "compiler.h"

int main() {
    x86::Compiler c;

    c.rdata("string", "Hello, World!");

    c.externalFunction("puts");

    c.function("f");

    c.push(x86::EBP);
    c.mov(x86::ESP, x86::EBP);
    c.push(c.abs("string"));
    c.call(c.rel("puts"));
    c.leave();
    c.ret();

    c.writeOBJ().write("a.o");

    // system("objdump -x a.o");
    // system("objdump -d a.o");

    return 0;
}
