#pragma once

#include "bytearray.h"

namespace vm {
class Function {
public:
    ByteArray code;

public:
    void gen(const char *opcodes);

    void invoke();
};
}
