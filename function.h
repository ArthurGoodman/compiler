#pragma once

#include <string>
#include <vector>

#include "bytearray.h"

namespace vm {
class Function {
public:
    ByteArray code;

public:
    Function();
    Function(const Function &f);
    Function(Function &&f);

    Function &operator=(const Function &f);
    Function &operator=(Function &&f);

    void gen(const char *opcodes);

    template <class T>
    void gen(T value);

    int invoke();
    int invoke(const std::vector<int> &args);

    std::string dump();
};

template <class T>
void Function::gen(T value) {
    *code.allocate(sizeof(T)) = value;
}
}
