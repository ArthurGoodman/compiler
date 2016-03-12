#include <iostream>

#include "testrunner.h"
#include "movtest.h"
#include "addtest.h"

#include "compiler.h"

int main() {
    unit::TestRunner runner;

    runner.addTest(new MovTest);
    runner.addTest(new AddTest);

    runner.run();

    //    vm::Compiler c;
    //    vm::Function f;

    //    c.push(vm::EBP);
    //    c.mov(vm::EBP, vm::ESP);
    //    c.mov(vm::EDX, vm::EBP + (byte)0x8);
    //    c.mov(vm::EAX, vm::EBP + (byte)0xc);
    //    c.add(vm::EAX, vm::EDX);
    //    c.leave();
    //    c.ret();

    //    f = c.compile();
    //    std::cout << f.dump() << "\n";
    //    std::cout << f.invoke(2, 1, 3) << "\n";

    //    c.push(vm::EBP);
    //    c.mov(vm::EBP, vm::ESP);
    //    c.mov(vm::EAX, vm::EBP + (byte)0x8);
    //    c.mov(vm::EDX, vm::ptr(vm::EAX));
    //    c.mov(vm::EAX, vm::EBP + (byte)0xc);
    //    c.mov(vm::EAX, vm::ptr(vm::EAX));
    //    c.add(vm::EAX, vm::EDX);
    //    c.pop(vm::EBP);
    //    c.ret();

    //    f = c.compile();
    //    std::cout << f.dump() << "\n";
    //    int a = 3, b = 4;
    //    std::cout << f.invoke({(int)&a, (int)&b}) << "\n";

    return 0;
}
