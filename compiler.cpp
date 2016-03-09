#include "compiler.h"

namespace vm {
Compiler::Compiler() {
}

void Compiler::push(Register reg) {
    f.gen((byte)('\x50' + reg));
}

void Compiler::pop(Register reg) {
    f.gen((byte)('\x58' + reg));
}

void Compiler::mov(Register to, Register from) {
    f.gen((byte)('\x89'));
    modRM(Reg, from, to);
}

void Compiler::mov(Register to, Register from, byte disp8) {
    f.gen((byte)('\x8b'));
    modRM(Disp8, to, from);
    f.gen(disp8);
}

void Compiler::mov(Register to, Register from, int disp32) {
    f.gen((byte)('\x8b'));
    modRM(Disp32, to, from);
    f.gen(disp32);
}

void Compiler::add(Register a, Register b) {
    f.gen((byte)('\x01'));
    modRM(Reg, b, a);
}

void Compiler::ret() {
    f.gen((byte)'\xc3');
}

Function Compiler::compile() {
    return std::move(f);
}

void Compiler::modRM(Mod mod, Register rm, Register r) {
    f.gen((byte)(mod << 6 | rm << 3 | r));
}
}
