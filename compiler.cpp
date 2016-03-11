#include "compiler.h"

#include <iostream>

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

void Compiler::mov(const Register &reg, byte value) {
    regRMInstruction(0xc7, reg, vm::EAX);
    f.gen(value);
}

void Compiler::mov(const Register &reg, int value) {
    regRMInstruction(0xc7, reg, vm::EAX);
    f.gen(value);
}

void Compiler::mov(const Register &dst, RegisterValue src) {
    mov(dst, Register(src));
}

void Compiler::lea(const Register &dst, const Register &src) {
    if (!src.isAddress())
        return;

    regRMInstruction(0x8d - 0x2, dst, src);
}

void Compiler::add(const Register &op1, const Register &op2) {
    regRMInstruction(0x1, op1, op2);
}

void Compiler::add(const Register &reg, byte value) {
    regRMInstruction(0x83, reg, vm::EAX);
    f.gen(value);
}

void Compiler::add(const Register &reg, int value) {
    regRMInstruction(0x83, reg, vm::EAX);
    f.gen(value);
}

void Compiler::add(const Register &op1, RegisterValue op2) {
    add(op1, Register(op2));
}

void Compiler::sub(const Register &op1, const Register &op2) {
    regRMInstruction(0x29, op1, op2);
}

void Compiler::sub(const Register &reg, byte value) {
    regRMInstruction(0x83, reg, vm::EBP);
    f.gen(value);
}

void Compiler::sub(const Register &reg, int value) {
    regRMInstruction(0x83, reg, vm::EBP);
    f.gen(value);
}

void Compiler::sub(const Register &op1, RegisterValue op2) {
    sub(op1, Register(op2));
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

void Compiler::regRMInstruction(byte op, const Register &op1, const Register &op2) {
    if (op1.isAddress() && op2.isAddress()) {
        std::cerr << "error: too many memory references\n";
        return;
    }

    f.gen((byte)(op + (op1.isAddress() || !op2.isAddress() ? 0x0 : 0x2)));

    if (!op1.isAddress() && !op2.isAddress()) {
        modRegRM(Reg, op2, op1);
        return;
    }

    const Register &r = op1.isAddress() ? op2 : op1;
    const Register &rm = op1.isAddress() ? op1 : op2;

    switch (rm.getDispSize()) {
    case 0:
        modRegRM(Disp0, r, rm);
        break;

    case 1:
        modRegRM(Disp8, r, rm);
        f.gen(rm.getDisp().as.Byte);
        break;

    case 4:
        modRegRM(Disp32, r, rm);
        f.gen(rm.getDisp().as.Int);
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
