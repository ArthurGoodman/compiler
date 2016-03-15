#include "compiler.h"

#include <cassert>
#include <cmath>

void vm::Compiler::push(const MemRef &ref) {
    if (ref.isAddress())
        regRMInstruction(0xff, ref, ESI);
    else
        f.gen((byte)(0x50 + ref));
}

void vm::Compiler::push(byte value) {
    f.gen((byte)0x6a);
    f.gen(value);
}

void vm::Compiler::push(int value) {
    f.gen((byte)0x68);
    f.gen(value);
}

void vm::Compiler::push(Register reg) {
    push(MemRef(reg));
}

void vm::Compiler::pop(const MemRef &ref) {
    if (ref.isAddress())
        regRMInstruction(0x8f, ref, EAX);
    else
        f.gen((byte)(0x58 + ref));
}

void vm::Compiler::mov(const MemRef &dst, const MemRef &src) {
    regRMInstruction(0x89, dst, src);
}

void vm::Compiler::mov(const MemRef &ref, byte value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        f.gen((byte)(0xb0 + ref));

    f.gen(value);
}

void vm::Compiler::mov(const MemRef &ref, int value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        f.gen((byte)(0xb8 + ref));

    f.gen(value);
}

void vm::Compiler::mov(const MemRef &dst, Register src) {
    mov(dst, MemRef(src));
}

void vm::Compiler::lea(const MemRef &dst, const MemRef &src) {
    assert(src.isAddress() && "src should be an address");

    regRMInstruction(0x8d - 0x2, dst, src);
}

void vm::Compiler::add(const MemRef &op1, const MemRef &op2) {
    regRMInstruction(0x1, op1, op2);
}

void vm::Compiler::add(const MemRef &ref, byte value) {
    regRMInstruction(0x83, ref, EAX);
    f.gen(value);
}

void vm::Compiler::add(const MemRef &ref, int value) {
    regRMInstruction(0x81, ref, EAX);
    f.gen(value);
}

void vm::Compiler::add(const MemRef &op1, Register op2) {
    add(op1, MemRef(op2));
}

void vm::Compiler::sub(const MemRef &op1, const MemRef &op2) {
    regRMInstruction(0x29, op1, op2);
}

void vm::Compiler::sub(const MemRef &ref, byte value) {
    regRMInstruction(0x83, ref, EBP);
    f.gen(value);
}

void vm::Compiler::sub(const MemRef &ref, int value) {
    regRMInstruction(0x83, ref, EBP);
    f.gen(value);
}

void vm::Compiler::sub(const MemRef &op1, Register op2) {
    sub(op1, MemRef(op2));
}

void vm::Compiler::leave() {
    f.gen((byte)0xc9);
}

void vm::Compiler::ret() {
    f.gen((byte)0xc3);
}

void vm::Compiler::nop() {
    f.gen((byte)0x90);
}

vm::Function vm::Compiler::compile() {
    return std::move(f);
}

void vm::Compiler::regRMInstruction(byte op, const MemRef &op1, const MemRef &op2) {
    assert(!(op1.isAddress() && op2.isAddress()) && "too many memory references");

    f.gen((byte)(op + (op2.isAddress() ? 0x2 : 0x0)));

    if (!op1.isAddress() && !op2.isAddress()) {
        modRegRM(Reg, op2, op1);
        return;
    }

    const MemRef &r = op1.isAddress() ? op2 : op1;
    const MemRef &rm = op1.isAddress() ? op1 : op2;

    switch (rm.getDispSize()) {
    case 0:
        if (rm.getScale() != 0) {
            modRegRM(Disp0, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp0, r, rm);
        break;

    case 1:
        if (rm.getScale() != 0) {
            modRegRM(Disp8, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp8, r, rm);
        f.gen((byte)rm.getDisp());
        break;

    case 4:
        if (rm.getBase() == NOREG)
            modRegRM(Disp0, r, EBP);
        else if (rm.getScale() != 0) {
            modRegRM(Disp32, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp32, r, rm);
        f.gen(rm.getDisp());
        break;

    default:
        break;
    }
}

void vm::Compiler::modRegRM(byte mod, byte reg, byte rm) {
    f.gen((byte)(mod << 6 | reg << 3 | rm));
}
