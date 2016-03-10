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

    bool isAddress();

    Disp getDisp();
    byte getDispSize();

    Register operator+(byte disp);
    Register operator+(int disp);

    operator byte();
};

Register operator+(RegisterValue value, byte disp);
Register operator+(RegisterValue value, int disp);

Register operator-(RegisterValue value, byte disp);
Register operator-(RegisterValue value, int disp);
}
