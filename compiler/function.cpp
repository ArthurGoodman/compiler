#include "function.h"

x86::Function::Function() {
}

x86::Function::Function(const Function &f)
    : code(f.code) {
}

x86::Function::Function(Function &&f)
    : code(std::move(f.code)) {
}

x86::Function &x86::Function::operator=(const Function &f) {
    code = f.code;
    return *this;
}

x86::Function &x86::Function::operator=(Function &&f) {
    code = std::move(f.code);
    return *this;
}

int x86::Function::invoke(int n, ...) {
    int (*f)() = (int (*)())code.getData();

    for (int *i = &n + n; i > &n; i--)
        asm("push %0"
            :
            : "g"(*i));

    int r = f();

    asm("add %0, %%esp"
        :
        : "g"(n * sizeof(int)));

    return r;
}

int x86::Function::invoke(const std::vector<int> &args) {
    int (*f)() = (int (*)())code.getData();

    const int *argsData = args.data();

    for (int i = args.size() - 1; i >= 0; i--)
        asm("push %0"
            :
            : "g"(argsData[i]));

    int r = f();

    asm("add %0, %%esp"
        :
        : "g"(args.size() * sizeof(int)));

    return r;
}

byte *x86::Function::getCode() {
    return code.getData();
}

std::string x86::Function::dump() {
    std::string result;

    for (uint i = 0; i < code.getSize(); i++)
        result += (i > 0 ? code[i] < 0x10 ? " 0" : " " : code[i] < 0x10 ? "0" : "") + toString((int)code[i], 0x10, 0);

    return result;
}
