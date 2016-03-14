#pragma once

#include "common.h"

namespace vm {
enum Register {
    NOREG = -1,

    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

class MemRef;

template <class T>
MemRef operator+(Register base, T disp);

template <class T>
MemRef operator+(const MemRef &ref, T disp);

MemRef operator+(Register base, const MemRef &ref);

template <class T>
MemRef operator-(Register base, T disp);

template <class T>
MemRef operator-(const MemRef &ref, T disp);

MemRef operator*(Register index, int scale);

MemRef ptr(Register base);
MemRef ptr(int disp);
}

class vm::MemRef {
    bool address;

    int scale;
    Register index;
    Register base;
    int disp;
    byte dispSize;

public:
    MemRef(Register base);
    MemRef(Register base, byte disp);
    MemRef(Register base, int disp);
    MemRef(int scale, Register index);
    MemRef(int scale, Register index, Register base, byte disp);
    MemRef(int scale, Register index, Register base, int disp);

    bool isAddress() const;
    MemRef &makeAddress();

    int getScale() const;
    Register getIndex() const;
    Register getBase() const;
    int getDisp() const;
    byte getDispSize() const;

    MemRef operator+(const MemRef &ref) const;

    operator byte() const;

private:
    MemRef(int scale, Register index, Register base, int disp, int dispSize);
};
