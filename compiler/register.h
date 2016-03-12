#pragma once

#include "common.h"

namespace vm {
enum RegisterValue {
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

class Register {
    struct Disp {
        union {
            byte Byte;
            int Int;
        } as;
    };

    RegisterValue value;

    bool address;

    Disp disp;
    byte dispSize;

public:
    Register(RegisterValue value);

    bool isAddress() const;
    Register &makeAddress();

    Disp getDisp() const;
    byte getDispSize() const;

    Register operator+(byte disp) const;
    Register operator+(int disp) const;

    operator byte() const;
};

Register operator+(RegisterValue value, byte disp);
Register operator+(RegisterValue value, int disp);

Register operator-(RegisterValue value, byte disp);
Register operator-(RegisterValue value, int disp);

Register ptr(const Register &reg);
}
