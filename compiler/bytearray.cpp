#include "bytearray.h"

#include <memory>

uint ByteArray::initialCapacity = 1;

void ByteArray::setInitialCapacity(uint initialCapacity) {
    ByteArray::initialCapacity = initialCapacity;
}

ByteArray::ByteArray()
    : size(0), capacity(0), data(0) {
}

ByteArray::ByteArray(const ByteArray &array)
    : size(array.size), capacity(array.capacity) {
    data = (byte *)malloc(capacity);
    memcpy(data, array.data, size);
}

ByteArray::ByteArray(ByteArray &&array)
    : size(array.size), capacity(array.capacity), data(array.data) {
    array.data = 0;
    array.size = 0;
    array.capacity = 0;
}

ByteArray::~ByteArray() {
    release();
}

ByteArray &ByteArray::operator=(const ByteArray &array) {
    size = array.size;
    capacity = array.capacity;

    ::free(data);
    data = (byte *)malloc(capacity);
    memcpy(data, array.data, size);

    return *this;
}

ByteArray &ByteArray::operator=(ByteArray &&array) {
    size = array.size;
    capacity = array.capacity;

    ::free(data);
    data = array.data;

    array.data = 0;
    array.size = 0;
    array.capacity = 0;

    return *this;
}

byte *ByteArray::allocate(uint count) {
    if (!enoughSpace(count)) {
        uint newCapacity = ceilToPowerOf2(std::max(initialCapacity, size + count));

        byte *newData = (byte *)realloc(data, newCapacity);

        if (!newData)
            return 0;

        capacity = newCapacity;
        data = newData;
    }

    size += count;

    return data + size - count;
}

int ByteArray::reallocate() {
    byte *newData = (byte *)malloc(capacity);

    memcpy(newData, data, size);

    int delta = newData - data;

    ::free(data);
    data = newData;

    return delta;
}

byte &ByteArray::operator[](int index) {
    return data[index];
}

bool ByteArray::free(uint count) {
    if (size < count)
        return false;

    size -= count;
    return true;
}

void ByteArray::release() {
    ::free(data);

    size = 0;
    capacity = 0;
    data = 0;
}

bool ByteArray::enoughSpace(uint count) const {
    return size + count <= capacity;
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
