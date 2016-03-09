#include "compiler.h"

namespace vm {
Compiler::Compiler() {
}

void Compiler::push(Register reg) {
    f.gen((byte)(0x50 + reg));
}

void Compiler::pop(Register reg) {
    f.gen((byte)(0x58 + reg));
}

void Compiler::mov(Register to, Register from) {
    f.gen((byte)0x89);
    modRM(Reg, from, to);
}

void Compiler::mov(Register to, Register from, byte disp8) {
    f.gen((byte)0x8b);
    modRM(Disp8, to, from);
    f.gen(disp8);
}

void Compiler::mov(Register to, Register from, int disp32) {
    f.gen((byte)0x8b);
    modRM(Disp32, to, from);
    f.gen(disp32);
}

void Compiler::add(Register a, Register b) {
    f.gen((byte)0x1);
    modRM(Reg, b, a);
}

void Compiler::add(Register a, Register b, byte disp8) {
    f.gen((byte)0x3);
    modRM(Disp8, a, b);
    f.gen(disp8);
}

void Compiler::add(Register a, Register b, int disp32) {
    f.gen((byte)0x3);
    modRM(Disp32, a, b);
    f.gen(disp32);
}

void Compiler::leave() {
    f.gen((byte)0xc9);
}

void Compiler::ret() {
    f.gen((byte)0xc3);
}

void Compiler::nop() {
    f.gen((byte)0x90);
}

Function Compiler::compile() {
    return std::move(f);
}

void Compiler::modRM(Mod mod, int r, int rm) {
    f.gen((byte)(mod << 6 | r << 3 | rm));

    if (rm == ESP)
        f.gen((byte)(0 << 6 | rm << 3 | rm));
}
}
