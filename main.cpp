#include <iostream>

#include "compiler.h"

int main() {
    vm::Compiler c;
    vm::Function f, g;

    c.push(vm::EBP);
    c.mov(vm::EBP, vm::ESP);
    c.mov(vm::EDX, vm::EBP, (byte)0x8);
    c.mov(vm::EAX, vm::EBP, (byte)0xc);
    c.add(vm::EAX, vm::EDX);
    c.leave();
    c.ret();

    f = c.compile();

    c.mov(vm::EAX, vm::ESP, (byte)0x8);
    c.add(vm::EAX, vm::ESP, (byte)0x4);
    c.ret();

    g = c.compile();

    std::cout << f.dump() << "\n";
    std::cout << g.dump() << "\n";

    std::cout << f.invoke(2, 3) << "\n";
    std::cout << g.invoke(4, 5) << "\n";

    return 0;
}
