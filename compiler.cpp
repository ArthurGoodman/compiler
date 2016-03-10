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

void Compiler::mov(Register dst, Register src) {
    if (dst.isAddress()) {
        if (src.isAddress())
            return;

        switch (dst.getDispSize()) {
        case 0:
            break;

        case 1:
            f.gen((byte)0x89);
            modRegRM(Disp8, dst, src);
            f.gen(dst.getDisp().as.Byte);
            break;

        case 4:
            f.gen((byte)0x89);
            modRegRM(Disp32, dst, src);
            f.gen(dst.getDisp().as.Int);
            break;

        default:
            return;
        }
    } else {
        if (src.isAddress()) {
            switch (src.getDispSize()) {
            case 0:
                break;

            case 1:
                f.gen((byte)0x8b);
                modRegRM(Disp8, dst, src);
                f.gen(src.getDisp().as.Byte);
                break;

            case 4:
                f.gen((byte)0x8b);
                modRegRM(Disp32, dst, src);
                f.gen(src.getDisp().as.Int);
                break;

            default:
                return;
            }
        } else {
            f.gen((byte)0x89);
            modRegRM(Reg, src, dst);
        }
    }
}

void Compiler::add(Register op1, Register op2) {
    if (op1.isAddress()) {
        if (op2.isAddress())
            return;

        switch (op1.getDispSize()) {
        case 0:
            break;

        case 1:
            f.gen((byte)0x1);
            modRegRM(Disp8, op1, op2);
            f.gen(op1.getDisp().as.Byte);
            break;

        case 4:
            f.gen((byte)0x1);
            modRegRM(Disp32, op1, op2);
            f.gen(op1.getDisp().as.Int);
            break;

        default:
            return;
        }
    } else {
        if (op2.isAddress()) {
            switch (op2.getDispSize()) {
            case 0:
                break;

            case 1:
                f.gen((byte)0x3);
                modRegRM(Disp8, op1, op2);
                f.gen(op2.getDisp().as.Byte);
                break;

            case 4:
                f.gen((byte)0x3);
                modRegRM(Disp32, op1, op2);
                f.gen(op2.getDisp().as.Int);
                break;

            default:
                return;
            }
        } else {
            f.gen((byte)0x1);
            modRegRM(Reg, op2, op1);
        }
    }
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

void Compiler::modRegRM(Mod mod, Register reg, Register rm) {
    f.gen(compose(mod, reg, rm));

    if (rm == ESP && mod != Reg)
        f.gen(compose(0, rm, rm));
}

byte Compiler::compose(byte first, byte second, byte third) {
    return first << 6 | second << 3 | third;
}
}
