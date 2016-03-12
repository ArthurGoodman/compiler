#include <iostream>

#include "unit.h"
#include "compiler.h"

int main() {
    run();

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
    std::cout << f.invoke(2, 1, 3) << "\n";

    return 0;
}
