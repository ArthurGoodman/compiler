#include "memref.h"

template x86::MemRef x86::operator+(Register base, byte disp);
template x86::MemRef x86::operator+(Register base, int disp);

template x86::MemRef x86::operator+(const MemRef &ref, byte disp);
template x86::MemRef x86::operator+(const MemRef &ref, int disp);

template x86::MemRef x86::operator-(Register base, byte disp);
template x86::MemRef x86::operator-(Register base, int disp);

template x86::MemRef x86::operator-(const MemRef &ref, byte disp);
template x86::MemRef x86::operator-(const MemRef &ref, int disp);

template <class T>
x86::MemRef x86::operator+(Register base, T disp) {
    return MemRef(base, disp);
}

template <class T>
x86::MemRef x86::operator+(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), disp);
}

x86::MemRef x86::operator+(Register base, const MemRef &ref) {
    return MemRef(base) + ref;
}

x86::MemRef x86::operator+(const x86::MemRef &ref, x86::Register base) {
    return ref + MemRef(base);
}

template <class T>
x86::MemRef x86::operator-(Register base, T disp) {
    return MemRef(base, (T)-disp);
}

template <class T>
x86::MemRef x86::operator-(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), (T)-disp);
}

x86::MemRef x86::operator*(Register index, int scale) {
    return MemRef(scale, index);
}

x86::MemRef x86::ptr(Register base) {
    return MemRef(base).makeAddress();
}

x86::MemRef x86::ptr(int disp) {
    return MemRef(NOREG, disp);
}

x86::MemRef::MemRef(Register base)
    : address(false), scale(0), index(NOREG), base(base), dispSize(0) {
}

x86::MemRef::MemRef(Register base, byte disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

x86::MemRef::MemRef(Register base, int disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

x86::MemRef::MemRef(int scale, Register index)
    : address(true), scale(scale), index(index), base(NOREG), dispSize(0) {
}

x86::MemRef::MemRef(int scale, Register index, Register base, byte disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

x86::MemRef::MemRef(int scale, Register index, Register base, int disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

bool x86::MemRef::isAddress() const {
    return address;
}

x86::MemRef &x86::MemRef::makeAddress() {
    address = true;
    return *this;
}

int x86::MemRef::getScale() const {
    return scale;
}

x86::Register x86::MemRef::getIndex() const {
    return index;
}

x86::Register x86::MemRef::getBase() const {
    return base;
}

int x86::MemRef::getDisp() const {
    return disp;
}

byte x86::MemRef::getDispSize() const {
    return dispSize;
}

x86::MemRef x86::MemRef::operator+(const MemRef &ref) const {
    if (scale != 0 && ref.scale == 0)
        return MemRef(scale, index, ref.base, disp + ref.disp, std::max(dispSize, ref.dispSize));
    else if (scale == 0 && ref.scale != 0)
        return MemRef(ref.scale, ref.index, base, disp + ref.disp, std::max(dispSize, ref.dispSize));

    return MemRef(NOREG);
}

x86::MemRef::operator byte() const {
    return base;
}

x86::MemRef::MemRef(int scale, Register index, Register base, int disp, int dispSize)
    : scale(scale), index(index), base(base), disp(disp), dispSize(dispSize) {
}
