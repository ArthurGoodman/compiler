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

    int invoke();
    int invoke(int _0);
    int invoke(int _0, int _1);
    int invoke(int _0, int _1, int _2);
    int invoke(int _0, int _1, int _2, int _3);
    int invoke(int _0, int _1, int _2, int _3, int _4);

    std::string dump();

private:
    void gen(const char *opcodes);

    template <class T>
    void gen(T value);
};

template <class T>
void Function::gen(T value) {
    *code.allocate(sizeof(T)) = value;
}
}
