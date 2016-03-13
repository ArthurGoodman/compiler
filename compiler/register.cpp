#include "register.h"

vm::Register::Register(RegisterValue value)
    : value(value), address(false), dispSize(0) {
}

bool vm::Register::isAddress() const {
    return address;
}

vm::Register &vm::Register::makeAddress() {
    address = true;
    return *this;
}

vm::Register::Disp vm::Register::getDisp() const {
    return disp;
}

byte vm::Register::getDispSize() const {
    return dispSize;
}

vm::Register vm::Register::operator+(byte disp) const {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Byte = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

vm::Register vm::Register::operator+(int disp) const {
    Register reg(value);

    reg.address = true;
    reg.disp.as.Int = disp;
    reg.dispSize = sizeof(disp);

    return reg;
}

vm::Register vm::Register::operator*(int scale) const {
    Register reg(value);

    reg.address = true;
    reg.scale = scale;

    return reg;
}

vm::RegisterValue vm::Register::getValue() {
    return value;
}

vm::RegisterValue vm::Register::getScaledValue() {
    return scaledValue;
}

vm::Register::operator byte() const {
    return value;
}

vm::Register vm::operator+(RegisterValue value, byte disp) {
    return Register(value) + disp;
}

vm::Register vm::operator+(RegisterValue value, int disp) {
    return Register(value) + disp;
}

vm::Register vm::operator-(RegisterValue value, byte disp) {
    return Register(value) + (byte)-disp;
}

vm::Register vm::operator-(RegisterValue value, int disp) {
    return Register(value) + -disp;
}

vm::Register vm::ptr(const Register &reg) {
    return Register(reg).makeAddress();
}
