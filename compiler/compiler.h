#pragma once

#include "function.h"
#include "memoryreference.h"

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

    void push(const MemoryReference &reg);
    void push(byte value);
    void push(int value);
    void push(Register reg);

    void pop(const MemoryReference &reg);

    void mov(const MemoryReference &dst, const MemoryReference &src);
    void mov(const MemoryReference &reg, byte value);
    void mov(const MemoryReference &reg, int value);
    void mov(const MemoryReference &dst, Register src);

    void lea(const MemoryReference &dst, const MemoryReference &src);

    void add(const MemoryReference &op1, const MemoryReference &op2);
    void add(const MemoryReference &reg, byte value);
    void add(const MemoryReference &reg, int value);
    void add(const MemoryReference &op1, Register op2);

    void sub(const MemoryReference &op1, const MemoryReference &op2);
    void sub(const MemoryReference &reg, byte value);
    void sub(const MemoryReference &reg, int value);
    void sub(const MemoryReference &op1, Register op2);

    void leave();
    void ret();

    void nop();

    Function compile();

private:
    void regRMInstruction(byte op, const MemoryReference &op1, const MemoryReference &op2);

    void modRegRM(Mod mod, const MemoryReference &reg, const MemoryReference &rm);
    byte compose(byte first, byte second, byte third);
};
}
