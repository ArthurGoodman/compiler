#include "register.h"

namespace vm {
Register::Register(RegisterValue value)
    : value(value), address(false), dispSize(0) {
}

bool Register::isAddress() const {
    return address;
}

Register::Disp Register::getDisp() const {
    return disp;
}

byte Register::getDispSize() const {
    return dispSize;
}

Register Register::operator+(byte disp) const {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Byte = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

Register Register::operator+(int disp) const {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Int = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

Register::operator byte() const {
    return value;
}

Register operator+(RegisterValue value, byte disp) {
    return Register(value) + disp;
}

Register operator+(RegisterValue value, int disp) {
    return Register(value) + disp;
}

Register operator-(RegisterValue value, byte disp) {
    return Register(value) + (byte)-disp;
}

Register operator-(RegisterValue value, int disp) {
    return Register(value) + -disp;
}
}
