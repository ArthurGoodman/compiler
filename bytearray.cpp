#include "bytearray.h"

#include <memory>
#include <cmath>

uint ByteArray::initialCapacity = 1;

void ByteArray::setInitialCapacity(uint initialCapacity) {
    ByteArray::initialCapacity = initialCapacity;
}

ByteArray::ByteArray()
    : size(0), capacity(0), data(0) {
}

ByteArray::~ByteArray() {
    release();
}

byte *ByteArray::allocate(uint count) {
    if (!enoughSpace(count)) {
        int newCapacity = capacity;

        if (!newCapacity)
            newCapacity = initialCapacity;

        newCapacity *= pow(2, std::max(0.0, ceil(log2(double(size + count) / newCapacity))));

        byte *newData = (byte *)realloc(data, newCapacity);

        if (!newData)
            return 0;

        capacity = newCapacity;
        data = newData;
    }

    size += count;

    return data + size - count;
}

bool ByteArray::free(uint count) {
    if (size < count)
        return false;

    size -= count;
    return true;
}

void ByteArray::release() {
    if (data)
        ::free(data);

    size = 0;
    capacity = 0;
    data = 0;
}

bool ByteArray::enoughSpace(uint count) const {
    return size + count <= capacity;
}

int ByteArray::reallocate() {
    byte *newData = (byte *)malloc(capacity);

    memcpy(newData, data, size);

    int delta = newData - data;

    ::free(data);
    data = newData;

    return delta;
}

byte *ByteArray::getData() const {
    return data;
}

uint ByteArray::getSize() const {
    return size;
}

uint ByteArray::getCapacity() const {
    return capacity;
}
