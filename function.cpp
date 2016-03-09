#include "function.h"

#include <string.h>

namespace vm {
Function::Function() {
}

Function::Function(const Function &f)
    : code(f.code) {
}

Function::Function(Function &&f)
    : code(std::move(f.code)) {
}

Function &Function::operator=(const Function &f) {
    code = f.code;
    return *this;
}

Function &Function::operator=(Function &&f) {
    code = std::move(f.code);
    return *this;
}

int Function::invoke() {
    return ((int (*)(...))code.getData())();
}

int Function::invoke(int _0) {
    return ((int (*)(...))code.getData())(_0);
}

int Function::invoke(int _0, int _1) {
    return ((int (*)(...))code.getData())(_0, _1);
}

int Function::invoke(int _0, int _1, int _2) {
    return ((int (*)(...))code.getData())(_0, _1, _2);
}

int Function::invoke(int _0, int _1, int _2, int _3) {
    return ((int (*)(...))code.getData())(_0, _1, _2, _3);
}

int Function::invoke(int _0, int _1, int _2, int _3, int _4) {
    return ((int (*)(...))code.getData())(_0, _1, _2, _3, _4);
}

std::string Function::dump() {
    std::string result;

    for (uint i = 0; i < code.getSize(); i++)
        result += (i > 0 ? code[i] < 0x10 ? " 0" : " " : "") + toString((int)code[i], 16, 0);

    return result;
}

void Function::gen(const char *opcodes) {
    int len = strlen(opcodes);
    byte *data = code.allocate(len);
    memcpy(data, opcodes, len);
}
}
