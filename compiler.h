#pragma once

#include "function.h"
#include "vm.h"

namespace vm {
class Compiler {
    Function f;

public:
    Compiler();

    void push(Register reg);
    void pop(Register reg);

    void mov(Register to, Register from);
    void mov(Register to, Register from, byte disp8);
    void mov(Register to, Register from, int disp32);

    void add(Register a, Register b);
    void add(Register a, Register b, byte disp8);
    void add(Register a, Register b, int disp32);

    void leave();
    void ret();

    void nop();

    Function compile();

private:
    void modRM(Mod mod, int r, int rm);
};
}
