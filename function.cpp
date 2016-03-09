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

void Function::gen(const char *opcodes) {
    int len = strlen(opcodes);
    byte *data = code.allocate(len);
    memcpy(data, opcodes, len);
}

int Function::invoke() {
    return ((int (*)())code.getData())();
}

int Function::invoke(const std::vector<int> &args) {
    switch (args.size()) {
    case 0:
        return ((int (*)(...))code.getData())();
    case 1:
        return ((int (*)(...))code.getData())(args[0]);
    case 2:
        return ((int (*)(...))code.getData())(args[0], args[1]);
    case 3:
        return ((int (*)(...))code.getData())(args[0], args[1], args[2]);
    case 4:
        return ((int (*)(...))code.getData())(args[0], args[1], args[2], args[3]);
    case 5:
        return ((int (*)(...))code.getData())(args[0], args[1], args[2], args[3], args[4]);
    default:
        return 0;
    }
}

std::string Function::dump() {
    std::string result;

    for (uint i = 0; i < code.getSize(); i++)
        result += "\\x" + toString((int)code[i], 16, 2);

    return result;
}
}
