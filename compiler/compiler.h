#pragma once

#include "function.h"
#include "memref.h"

namespace vm {
class Compiler;
}

class vm::Compiler {
    enum Mod {
        Disp0,
        Disp8,
        Disp32,
        Reg
    };

    Function f;

public:
    void push(const MemRef &ref);
    void push(byte value);
    void push(int value);
    void push(Register reg);

    void pop(const MemRef &ref);

    void mov(const MemRef &dst, const MemRef &src);
    void mov(const MemRef &ref, byte value);
    void mov(const MemRef &ref, int value);
    void mov(const MemRef &dst, Register src);

    void lea(const MemRef &dst, const MemRef &src);

    void add(const MemRef &op1, const MemRef &op2);
    void add(const MemRef &ref, byte value);
    void add(const MemRef &ref, int value);
    void add(const MemRef &op1, Register op2);

    void sub(const MemRef &op1, const MemRef &op2);
    void sub(const MemRef &ref, byte value);
    void sub(const MemRef &ref, int value);
    void sub(const MemRef &op1, Register op2);

    void leave();
    void ret();

    void nop();

    Function compile();

private:
    void regRMInstruction(byte op, const MemRef &op1, const MemRef &op2);
    void modRegRM(byte mod, byte reg, byte rm);
};
