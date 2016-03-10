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

int Function::invoke(int n, ...) {
    int (*f)() = (int (*)())code.getData();

    for (int i = 0; i < n; i++)
        asm("push %0\n"
            :
            : "g"(*(&n + i + 1)));

    int r = f();

    asm("add esp,%0\n"
        :
        : "g"(n * sizeof(int)));

    return r;
}

int Function::invoke(const std::vector<int> &args) {
    int (*f)() = (int (*)())code.getData();

    for (uint i = 0; i < args.size(); i++)
        asm("push %0\n"
            :
            : "g"(args[i]));

    int r = f();

    asm("add esp,%0\n"
        :
        : "g"(args.size() * sizeof(int)));

    return r;
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
