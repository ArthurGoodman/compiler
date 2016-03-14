#include "compiler.h"

#include <cassert>
#include <cmath>

vm::Compiler::Compiler() {
}

void vm::Compiler::push(const MemoryReference &ref) {
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
    push(MemoryReference(reg));
}

void vm::Compiler::pop(const MemoryReference &ref) {
    if (ref.isAddress())
        regRMInstruction(0x8f, ref, EAX);
    else
        f.gen((byte)(0x58 + ref));
}

void vm::Compiler::mov(const MemoryReference &dst, const MemoryReference &src) {
    regRMInstruction(0x89, dst, src);
}

void vm::Compiler::mov(const MemoryReference &ref, byte value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        f.gen((byte)(0xb0 + ref));

    f.gen(value);
}

void vm::Compiler::mov(const MemoryReference &ref, int value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        f.gen((byte)(0xb8 + ref));

    f.gen(value);
}

void vm::Compiler::mov(const MemoryReference &dst, Register src) {
    mov(dst, MemoryReference(src));
}

void vm::Compiler::lea(const MemoryReference &dst, const MemoryReference &src) {
    assert(src.isAddress() && "src should be an address");

    regRMInstruction(0x8d - 0x2, dst, src);
}

void vm::Compiler::add(const MemoryReference &op1, const MemoryReference &op2) {
    regRMInstruction(0x1, op1, op2);
}

void vm::Compiler::add(const MemoryReference &ref, byte value) {
    regRMInstruction(0x83, ref, EAX);
    f.gen(value);
}

void vm::Compiler::add(const MemoryReference &ref, int value) {
    regRMInstruction(0x81, ref, EAX);
    f.gen(value);
}

void vm::Compiler::add(const MemoryReference &op1, Register op2) {
    add(op1, MemoryReference(op2));
}

void vm::Compiler::sub(const MemoryReference &op1, const MemoryReference &op2) {
    regRMInstruction(0x29, op1, op2);
}

void vm::Compiler::sub(const MemoryReference &ref, byte value) {
    regRMInstruction(0x83, ref, EBP);
    f.gen(value);
}

void vm::Compiler::sub(const MemoryReference &ref, int value) {
    regRMInstruction(0x83, ref, EBP);
    f.gen(value);
}

void vm::Compiler::sub(const MemoryReference &op1, Register op2) {
    sub(op1, MemoryReference(op2));
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

void vm::Compiler::regRMInstruction(byte op, const MemoryReference &op1, const MemoryReference &op2) {
    assert(!(op1.isAddress() && op2.isAddress()) && "too many memory references");

    f.gen((byte)(op + (op2.isAddress() ? 0x2 : 0x0)));

    if (!op1.isAddress() && !op2.isAddress()) {
        modRegRM(Reg, op2, op1);
        return;
    }

    const MemoryReference &r = op1.isAddress() ? op2 : op1;
    const MemoryReference &rm = op1.isAddress() ? op1 : op2;

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
