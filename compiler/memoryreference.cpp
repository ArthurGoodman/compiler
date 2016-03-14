#include "memoryreference.h"

template vm::MemoryReference vm::operator+(Register base, byte disp);
template vm::MemoryReference vm::operator+(Register base, int disp);

template vm::MemoryReference vm::operator+(const MemoryReference &ref, byte disp);
template vm::MemoryReference vm::operator+(const MemoryReference &ref, int disp);

template vm::MemoryReference vm::operator-(Register base, byte disp);
template vm::MemoryReference vm::operator-(Register base, int disp);

template vm::MemoryReference vm::operator-(const MemoryReference &ref, byte disp);
template vm::MemoryReference vm::operator-(const MemoryReference &ref, int disp);

vm::MemoryReference::MemoryReference(Register base)
    : address(false), scale(0), index(NOREG), base(base), dispSize(0) {
}

vm::MemoryReference::MemoryReference(Register base, byte disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemoryReference::MemoryReference(Register base, int disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemoryReference::MemoryReference(int scale, Register index)
    : address(true), scale(scale), index(index), base(NOREG), dispSize(0) {
}

vm::MemoryReference::MemoryReference(int scale, Register index, Register base, byte disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemoryReference::MemoryReference(int scale, Register index, Register base, int disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

bool vm::MemoryReference::isAddress() const {
    return address;
}

vm::MemoryReference &vm::MemoryReference::makeAddress() {
    address = true;
    return *this;
}

int vm::MemoryReference::getScale() const {
    return scale;
}

void vm::MemoryReference::setScale(int scale) {
    this->scale = scale;
}

vm::Register vm::MemoryReference::getIndex() const {
    return index;
}

void vm::MemoryReference::setIndex(Register index) {
    this->index = index;
}

vm::Register vm::MemoryReference::getBase() const {
    return base;
}

void vm::MemoryReference::setBase(Register base) {
    this->base = base;
}

int vm::MemoryReference::getDisp() const {
    return disp;
}

void vm::MemoryReference::setDisp(byte disp) {
    this->disp = disp;
    dispSize = sizeof(disp);
}

void vm::MemoryReference::setDisp(int disp) {
    this->disp = disp;
    dispSize = sizeof(disp);
}

byte vm::MemoryReference::getDispSize() const {
    return dispSize;
}

vm::MemoryReference vm::MemoryReference::operator+(const MemoryReference &ref) const {
    if (scale != 0 && ref.scale == 0)
        return MemoryReference(scale, index, ref.base, disp + ref.disp, std::max(dispSize, ref.dispSize));
    else if (scale == 0 && ref.scale != 0)
        return MemoryReference(ref.scale, ref.index, base, disp + ref.disp, std::max(dispSize, ref.dispSize));

    return MemoryReference(NOREG);
}

vm::MemoryReference::operator byte() const {
    return base;
}

vm::MemoryReference::MemoryReference(int scale, Register index, Register base, int disp, int dispSize)
    : scale(scale), index(index), base(base), disp(disp), dispSize(dispSize) {
}

template <class T>
vm::MemoryReference vm::operator+(Register base, T disp) {
    return MemoryReference(base, disp);
}

vm::MemoryReference vm::operator+(Register base, const MemoryReference &ref) {
    return MemoryReference(base) + ref;
}

template <class T>
vm::MemoryReference vm::operator+(const MemoryReference &ref, T disp) {
    return MemoryReference(ref.getScale(), ref.getIndex(), ref.getBase(), disp);
}

template <class T>
vm::MemoryReference vm::operator-(Register base, T disp) {
    return MemoryReference(base, (T)-disp);
}

template <class T>
vm::MemoryReference vm::operator-(const MemoryReference &ref, T disp) {
    return MemoryReference(ref.getScale(), ref.getIndex(), ref.getBase(), (T)-disp);
}

vm::MemoryReference vm::operator*(Register index, int scale) {
    return MemoryReference(scale, index);
}

vm::MemoryReference vm::ptr(const MemoryReference &ref) {
    return MemoryReference(ref).makeAddress();
}

vm::MemoryReference vm::ptr(Register base) {
    return MemoryReference(base).makeAddress();
}

vm::MemoryReference vm::ptr(int disp) {
    return MemoryReference(NOREG, disp);
}
