#include <iostream>

#include "compiler.h"

int main() {
    x86::Compiler c;

    c.rdata("string", "Hello, World!");

    c.external("puts");

    c.function("f");

    c.push(x86::EBP);
    c.mov(x86::ESP, x86::EBP);
    c.push(0);
    c.nop(); // c.call(0);
    c.leave();
    c.ret();

    c.writeOBJ().write("a.o");

    return 0;
}
