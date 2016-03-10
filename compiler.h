#pragma once

#include "function.h"
#include "register.h"

namespace vm {
class Compiler {
    enum Mod {
        Disp0,
        Disp8,
        Disp32,
        Reg
    };

    Function f;

public:
    Compiler();

    void push(Register reg);
    void pop(Register reg);

    void mov(Register dst, Register src);

    void add(Register op1, Register op2);

    void leave();
    void ret();

    void nop();

    Function compile();

private:
    void modRegRM(Mod mod, Register reg, Register rm);
    byte compose(byte first, byte second, byte third);
};
}
