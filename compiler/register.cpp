#include "register.h"

template vm::Register vm::operator+(RegisterValue base, byte disp);
template vm::Register vm::operator+(RegisterValue base, int disp);

template vm::Register vm::operator+(const Register &reg, byte disp);
template vm::Register vm::operator+(const Register &reg, int disp);

template vm::Register vm::operator-(RegisterValue base, byte disp);
template vm::Register vm::operator-(RegisterValue base, int disp);

template vm::Register vm::operator-(const Register &reg, byte disp);
template vm::Register vm::operator-(const Register &reg, int disp);

vm::Register::Register(RegisterValue base)
    : address(false), scale(0), index(NOREG), base(base), dispSize(0) {
}

vm::Register::Register(RegisterValue base, byte disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::Register::Register(RegisterValue base, int disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::Register::Register(int scale, RegisterValue index)
    : address(true), scale(scale), index(index), base(NOREG), dispSize(0) {
}

vm::Register::Register(int scale, RegisterValue index, RegisterValue base, byte disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::Register::Register(int scale, RegisterValue index, RegisterValue base, int disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

bool vm::Register::isAddress() const {
    return address;
}

vm::Register &vm::Register::makeAddress() {
    address = true;
    return *this;
}

int vm::Register::getScale() const {
    return scale;
}

void vm::Register::setScale(int scale) {
    this->scale = scale;
}

vm::RegisterValue vm::Register::getIndex() const {
    return index;
}

void vm::Register::setIndex(RegisterValue index) {
    this->index = index;
}

vm::RegisterValue vm::Register::getBase() const {
    return base;
}

void vm::Register::setBase(vm::RegisterValue base) {
    this->base = base;
}

int vm::Register::getDisp() const {
    return disp;
}

void vm::Register::setDisp(byte disp) {
    this->disp = disp;
    dispSize = sizeof(disp);
}

void vm::Register::setDisp(int disp) {
    this->disp = disp;
    dispSize = sizeof(disp);
}

byte vm::Register::getDispSize() const {
    return dispSize;
}

vm::Register vm::Register::operator+(const Register &reg) const {
    if (scale != 0 && reg.scale == 0)
        return Register(scale, index, reg.base, disp + reg.disp, std::max(dispSize, reg.dispSize));
    else if (scale == 0 && reg.scale != 0)
        return Register(reg.scale, reg.index, base, disp + reg.disp, std::max(dispSize, reg.dispSize));

    return Register(NOREG);
}

vm::Register::operator byte() const {
    return base;
}

vm::Register::Register(int scale, RegisterValue index, RegisterValue base, int disp, int dispSize)
    : scale(scale), index(index), base(base), disp(disp), dispSize(dispSize) {
}

template <class T>
vm::Register vm::operator+(RegisterValue base, T disp) {
    return Register(base, disp);
}

template <class T>
vm::Register vm::operator+(const Register &reg, T disp) {
    return Register(reg.getScale(), reg.getIndex(), reg.getBase(), disp);
}

template <class T>
vm::Register vm::operator-(RegisterValue base, T disp) {
    return Register(base, (T)-disp);
}

template <class T>
vm::Register vm::operator-(const Register &reg, T disp) {
    return Register(reg.getScale(), reg.getIndex(), reg.getBase(), (T)-disp);
}

vm::Register vm::operator*(RegisterValue index, uint scale) {
    return Register(scale, index);
}

vm::Register vm::ptr(const Register &reg) {
    return Register(reg).makeAddress();
}
