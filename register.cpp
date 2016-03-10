#include "register.h"

namespace vm {
Register::Register(RegisterValue value)
    : value(value), address(false), dispSize(0) {
}

bool Register::isAddress() {
    return address;
}

Register::Disp Register::getDisp() {
    return disp;
}

byte Register::getDispSize() {
    return dispSize;
}

Register Register::operator+(byte disp) {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Byte = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

Register Register::operator+(int disp) {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Int = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

Register::operator byte() {
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
