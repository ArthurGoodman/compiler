#pragma once

#include <string>
#include <vector>

#include "bytearray.h"

namespace vm {
class Function {
    friend class Compiler;

    ByteArray code;

public:
    Function();
    Function(const Function &f);
    Function(Function &&f);

    Function &operator=(const Function &f);
    Function &operator=(Function &&f);

    int invoke(int n = 0, ...);
    std::string dump();

private:
    void gen(const char *opcodes);

    template <class T>
    void gen(T value);
};

template <class T>
void Function::gen(T value) {
    *(T *)code.allocate(sizeof(T)) = value;
}
}
