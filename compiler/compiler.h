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

    void push(const Register &reg);
    void pop(const Register &reg);

    void mov(const Register &dst, const Register &src);
    void mov(const Register &reg, byte value);
    void mov(const Register &reg, int value);
    void mov(const Register &dst, RegisterValue src);

    void lea(const Register &dst, const Register &src);

    void add(const Register &op1, const Register &op2);
    void add(const Register &reg, byte value);
    void add(const Register &reg, int value);
    void add(const Register &op1, RegisterValue op2);

    void sub(const Register &op1, const Register &op2);
    void sub(const Register &reg, byte value);
    void sub(const Register &reg, int value);
    void sub(const Register &op1, RegisterValue op2);

    void leave();
    void ret();

    void nop();

    Function compile();

private:
    void regRMInstruction(byte op, const Register &op1, const Register &op2);

    void modRegRM(Mod mod, const Register &reg, const Register &rm);
    byte compose(byte first, byte second, byte third);
};
}
