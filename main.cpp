#include <iostream>

#include "compiler.h"

int main() {
    vm::Compiler c;
    vm::Function f;

    c.push(vm::EBP);
    c.mov(vm::EBP, vm::ESP);
    c.mov(vm::EDX, vm::EBP + (byte)0x8);
    c.mov(vm::EAX, vm::EBP + (byte)0xc);
    c.add(vm::EAX, vm::EDX);
    c.leave();
    c.ret();

    f = c.compile();
    std::cout << f.dump() << "\n";
    std::cout << f.invoke(2, 2, 3) << "\n";

    c.mov(vm::EAX, vm::ESP + (byte)0x8);
    c.add(vm::EAX, vm::ESP + (byte)0x4);
    c.ret();

    f = c.compile();
    std::cout << f.dump() << "\n";
    std::cout << f.invoke({4, 5}) << "\n";

    std::cout << c.compile().dump() << "\n";

    return 0;
}
