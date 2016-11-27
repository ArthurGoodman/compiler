#include "memref.h"

namespace x86 {

template MemRef operator+(Register base, byte disp);
template MemRef operator+(Register base, int disp);

template MemRef operator+(const MemRef &ref, byte disp);
template MemRef operator+(const MemRef &ref, int disp);

template MemRef operator-(Register base, byte disp);
template MemRef operator-(Register base, int disp);

template MemRef operator-(const MemRef &ref, byte disp);
template MemRef operator-(const MemRef &ref, int disp);

template <class T>
MemRef operator+(Register base, T disp) {
    return MemRef(base, disp);
}

template <class T>
MemRef operator+(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), disp);
}

MemRef operator+(Register base, const MemRef &ref) {
    return MemRef(base) + ref;
}

MemRef operator+(const MemRef &ref, Register base) {
    return ref + MemRef(base);
}

template <class T>
MemRef operator-(Register base, T disp) {
    return MemRef(base, (T)-disp);
}

template <class T>
MemRef operator-(const MemRef &ref, T disp) {
    return MemRef(ref.getScale(), ref.getIndex(), ref.getBase(), (T)-disp);
}

MemRef operator*(Register index, int scale) {
    return MemRef(scale, index);
}

MemRef ptr(Register base) {
    return MemRef(base).makeAddress();
}

MemRef ptr(int disp) {
    return MemRef(NOREG, disp);
}

MemRef::MemRef(Register base)
    : address(false), scale(0), index(NOREG), base(base), dispSize(0) {
}

MemRef::MemRef(Register base, byte disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

MemRef::MemRef(Register base, int disp)
    : address(true), scale(0), index(NOREG), base(base), disp(disp), dispSize(sizeof(disp)) {
}

MemRef::MemRef(int scale, Register index)
    : address(true), scale(scale), index(index), base(NOREG), dispSize(0) {
}

MemRef::MemRef(int scale, Register index, Register base, byte disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

MemRef::MemRef(int scale, Register index, Register base, int disp)
    : address(true), scale(scale), index(index), base(base), disp(disp), dispSize(sizeof(disp)) {
}

bool MemRef::isAddress() const {
    return address;
}

MemRef &MemRef::makeAddress() {
    address = true;
    return *this;
}

int MemRef::getScale() const {
    return scale;
}

Register MemRef::getIndex() const {
    return index;
}

Register MemRef::getBase() const {
    return base;
}

int MemRef::getDisp() const {
    return disp;
}

byte MemRef::getDispSize() const {
    return dispSize;
}

MemRef MemRef::operator+(const MemRef &ref) const {
    if (scale != 0 && ref.scale == 0)
        return MemRef(scale, index, ref.base, disp + ref.disp, std::max(dispSize, ref.dispSize));
    else if (scale == 0 && ref.scale != 0)
        return MemRef(ref.scale, ref.index, base, disp + ref.disp, std::max(dispSize, ref.dispSize));

    return MemRef(NOREG);
}

MemRef::operator byte() const {
    return base;
}

MemRef::MemRef(int scale, Register index, Register base, int disp, int dispSize)
    : scale(scale), index(index), base(base), disp(disp), dispSize(dispSize) {
}
}
