#include "compiler.h"

namespace vm {
Compiler::Compiler() {
}

void Compiler::push(const Register &reg) {
    f.gen((byte)(0x50 + reg));
}

void Compiler::pop(const Register &reg) {
    f.gen((byte)(0x58 + reg));
}

void Compiler::mov(const Register &dst, const Register &src) {
    regRMInstruction(0x89, dst, src);
}

void Compiler::add(const Register &op1, const Register &op2) {
    regRMInstruction(0x1, op1, op2);
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

void Compiler::regRMInstruction(byte op, const Register &dst, const Register &src) {
    if (dst.isAddress() && src.isAddress())
        return;

    f.gen((byte)(op + (dst.isAddress() || !src.isAddress() ? 0x0 : 0x2)));

    if (!dst.isAddress() && !src.isAddress()) {
        modRegRM(Reg, src, dst);
        return;
    }

    switch (dst.isAddress() ? dst.getDispSize() : src.getDispSize()) {
    case 0:
        break;

    case 1:
        modRegRM(Disp8, dst, src);
        if (dst.isAddress() || src.isAddress())
            f.gen(dst.isAddress() ? dst.getDisp().as.Byte : src.getDisp().as.Byte);
        break;

    case 4:
        modRegRM(Disp32, dst, src);
        if (dst.isAddress() || src.isAddress())
            f.gen(dst.isAddress() ? dst.getDisp().as.Int : src.getDisp().as.Int);
        break;

    default:
        break;
    }
}

void Compiler::modRegRM(Mod mod, const Register &reg, const Register &rm) {
    f.gen(compose(mod, reg, rm));

    if (rm == ESP && mod != Reg)
        f.gen(compose(0, rm, rm));
}

byte Compiler::compose(byte first, byte second, byte third) {
    return first << 6 | second << 3 | third;
}
}
