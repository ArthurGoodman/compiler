#pragma once

#include <string>
#include <vector>

#include "bytearray.h"

namespace vm {
class Function;
}

class vm::Function {
    friend class Compiler;

    ByteArray code;

public:
    Function();

    Function(const Function &f);
    Function(Function &&f);

    Function &operator=(const Function &f);
    Function &operator=(Function &&f);

    int invoke(int n = 0, ...);
    int invoke(const std::vector<int> &args);

    byte *getCode();

    std::string dump();

private:
    template <class T>
    void gen(T value);
};

template <class T>
void vm::Function::gen(T value) {
    code.push(value);
}
