#pragma once

#include "common.h"

namespace vm {
enum RegisterValue {
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

class Register {
    bool address;

    int scale;
    RegisterValue index;
    RegisterValue base;
    int disp;
    byte dispSize;

public:
    Register(RegisterValue base);
    Register(RegisterValue base, byte disp);
    Register(RegisterValue base, int disp);
    Register(int scale, RegisterValue index);
    Register(int scale, RegisterValue index, RegisterValue base, byte disp);
    Register(int scale, RegisterValue index, RegisterValue base, int disp);

    bool isAddress() const;
    Register &makeAddress();

    int getScale() const;
    void setScale(int scale);

    RegisterValue getIndex() const;
    void setIndex(RegisterValue index);

    RegisterValue getBase() const;
    void setBase(RegisterValue base);

    int getDisp() const;
    void setDisp(byte disp);
    void setDisp(int disp);

    byte getDispSize() const;

    Register operator+(const Register &reg) const;

    operator byte() const;

private:
    Register(int scale, RegisterValue index, RegisterValue base, int disp, int dispSize);
};

template <class T>
Register operator+(RegisterValue base, T disp);

template <class T>
Register operator+(const Register &reg, T disp);

template <class T>
Register operator-(RegisterValue base, T disp);

template <class T>
Register operator-(const Register &reg, T disp);

Register operator*(RegisterValue index, uint scale);

Register ptr(const Register &reg);
}
