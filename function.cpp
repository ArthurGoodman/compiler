#include "function.h"

#include <string.h>

namespace vm {
void Function::gen(const char *opcodes) {
    int len = strlen(opcodes);
    byte *data = code.allocate(len);
    memcpy(data, opcodes, len);
}

void Function::invoke() {
    ((void (*)())code.getData())();
}
}
