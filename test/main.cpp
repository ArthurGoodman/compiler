#include <iostream>

#include "compiler.h"

int main() {
    x86::Compiler c;

    c.rdata("string", "Hello, World!");

    c.externalFunction("puts");

    c.function("f");

    c.push(x86::EBP);
    c.mov(x86::EBP, x86::ESP);
    c.push(c.abs("string"));
    c.call(c.rel("puts"));
    c.leave();
    c.ret();

    c.writeOBJ().write("a.o");

    return 0;
}
