#include "compilertest.h"

#include <iostream>

CompilerTest::CompilerTest(const std::string &name)
    : TestSet(name) {
}

void CompilerTest::$(const std::string &name, const std::string &result, const std::function<void()> &f) {
    TestSet::$(name, [=]() {
        f();
        std::cout << c.compile().dump();
    }).should.print(result);
}
