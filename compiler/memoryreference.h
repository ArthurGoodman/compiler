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

class MemoryReference {
    bool address;

    int scale;
    Register index;
    Register base;
    int disp;
    byte dispSize;

public:
    MemoryReference(Register base);
    MemoryReference(Register base, byte disp);
    MemoryReference(Register base, int disp);
    MemoryReference(int scale, Register index);
    MemoryReference(int scale, Register index, Register base, byte disp);
    MemoryReference(int scale, Register index, Register base, int disp);

    bool isAddress() const;
    MemoryReference &makeAddress();

    int getScale() const;
    void setScale(int scale);

    Register getIndex() const;
    void setIndex(Register index);

    Register getBase() const;
    void setBase(Register base);

    int getDisp() const;
    void setDisp(byte disp);
    void setDisp(int disp);

    byte getDispSize() const;

    MemoryReference operator+(const MemoryReference &ref) const;

    operator byte() const;

private:
    MemoryReference(int scale, Register index, Register base, int disp, int dispSize);
};

template <class T>
MemoryReference operator+(Register base, T disp);

MemoryReference operator+(Register base, const MemoryReference &ref);

template <class T>
MemoryReference operator+(const MemoryReference &ref, T disp);

template <class T>
MemoryReference operator-(Register base, T disp);

template <class T>
MemoryReference operator-(const MemoryReference &ref, T disp);

MemoryReference operator*(Register index, int scale);

MemoryReference ptr(const MemoryReference &ref);
MemoryReference ptr(Register base);
MemoryReference ptr(int disp);
}
