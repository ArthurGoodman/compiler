#include "memref.h"

template vm::MemRef vm::operator+(Register base, byte disp);
template vm::MemRef vm::operator+(Register base, int disp);

template vm::MemRef vm::operator+(const MemRef &ref, byte disp);
template vm::MemRef vm::operator+(const MemRef &ref, int disp);

template vm::MemRef vm::operator-(Register base, byte disp);
template vm::MemRef vm::operator-(Register base, int disp);

template vm::MemRef vm::operator-(const MemRef &ref, byte disp);
template vm::MemRef vm::operator-(const MemRef &ref, int disp);

template <class T>
vm::MemRef vm::operator+(Register base, T disp) {
    return MemRef(base, disp);
}

template <class T>
vm::MemRef vm::operator+(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), disp);
}

vm::MemRef vm::operator+(Register base, const MemRef &ref) {
    return MemRef(base) + ref;
}

vm::MemRef vm::operator+(const vm::MemRef &ref, vm::Register base) {
    return ref + MemRef(base);
}

template <class T>
vm::MemRef vm::operator-(Register base, T disp) {
    return MemRef(base, (T)-disp);
}

template <class T>
vm::MemRef vm::operator-(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), (T)-disp);
}

vm::MemRef vm::operator*(Register index, int scale) {
    return MemRef(scale, index);
}

vm::MemRef vm::ptr(Register base) {
    return MemRef(base).makeAddress();
}

vm::MemRef vm::ptr(int disp) {
    return MemRef(NOREG, disp);
}

vm::MemRef::MemRef(Register base)
    : address(false), scale(0), index(NOREG), base(base), dispSize(0) {
}

vm::MemRef::MemRef(Register base, byte disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemRef::MemRef(Register base, int disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemRef::MemRef(int scale, Register index)
    : address(true), scale(scale), index(index), base(NOREG), dispSize(0) {
}

vm::MemRef::MemRef(int scale, Register index, Register base, byte disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

vm::MemRef::MemRef(int scale, Register index, Register base, int disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

bool vm::MemRef::isAddress() const {
    return address;
}

vm::MemRef &vm::MemRef::makeAddress() {
    address = true;
    return *this;
}

int vm::MemRef::getScale() const {
    return scale;
}

vm::Register vm::MemRef::getIndex() const {
    return index;
}

vm::Register vm::MemRef::getBase() const {
    return base;
}

int vm::MemRef::getDisp() const {
    return disp;
}

byte vm::MemRef::getDispSize() const {
    return dispSize;
}

vm::MemRef vm::MemRef::operator+(const MemRef &ref) const {
    if (scale != 0 && ref.scale == 0)
        return MemRef(scale, index, ref.base, disp + ref.disp, std::max(dispSize, ref.dispSize));
    else if (scale == 0 && ref.scale != 0)
        return MemRef(ref.scale, ref.index, base, disp + ref.disp, std::max(dispSize, ref.dispSize));

    return MemRef(NOREG);
}

vm::MemRef::operator byte() const {
    return base;
}

vm::MemRef::MemRef(int scale, Register index, Register base, int disp, int dispSize)
    : scale(scale), index(index), base(base), disp(disp), dispSize(dispSize) {
}
