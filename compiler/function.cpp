#include "function.h"

vm::Function::Function() {
}

vm::Function::Function(const Function &f)
    : code(f.code) {
}

vm::Function::Function(Function &&f)
    : code(std::move(f.code)) {
}

vm::Function &vm::Function::operator=(const Function &f) {
    code = f.code;
    return *this;
}

vm::Function &vm::Function::operator=(Function &&f) {
    code = std::move(f.code);
    return *this;
}

int vm::Function::invoke(int n, ...) {
    int (*f)() = (int (*)())code.getData();

    for (int *i = &n + n; i > &n; i--)
        asm("push %0"
            :
            : "g"(*i));

    int r = f();

    asm("add esp,%0"
        :
        : "g"(n * sizeof(int)));

    return r;
}

int vm::Function::invoke(const std::vector<int> &args) {
    int (*f)() = (int (*)())code.getData();

    const int *argsData = args.data();

    for (int i = args.size() - 1; i >= 0; i--)
        asm("push %0"
            :
            : "g"(argsData[i]));

    int r = f();

    asm("add esp,%0"
        :
        : "g"(args.size() * sizeof(int)));

    return r;
}

byte *vm::Function::getCode() {
    return code.getData();
}

std::string vm::Function::dump() {
    std::string result;

    for (uint i = 0; i < code.getSize(); i++)
        result += (i > 0 ? code[i] < 0x10 ? " 0" : " " : code[i] < 0x10 ? "0" : "") + toString((int)code[i], 0x10, 0);

    return result;
}
