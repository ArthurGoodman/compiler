#include "bytearray.h"

#include <memory>
#include <fstream>

uint ByteArray::initialCapacity = 1;

void ByteArray::setInitialCapacity(uint initialCapacity) {
    ByteArray::initialCapacity = initialCapacity;
}

ByteArray::ByteArray()
    : _size(0), _capacity(0), _data(0) {
}

ByteArray::ByteArray(const ByteArray &array)
    : _data(0) {
    *this = array;
}

ByteArray::ByteArray(ByteArray &&array)
    : _data(0) {
    *this = std::move(array);
}

ByteArray::~ByteArray() {
    release();
}

ByteArray &ByteArray::operator=(const ByteArray &array) {
    _size = array._size;
    _capacity = array._capacity;

    ::free(_data);

    _data = (byte *)malloc(_capacity);
    memcpy(_data, array._data, _size);

    return *this;
}

ByteArray &ByteArray::operator=(ByteArray &&array) {
    _size = array._size;
    _capacity = array._capacity;

    ::free(_data);

    _data = array._data;

    array._data = 0;
    array._size = 0;
    array._capacity = 0;

    return *this;
}

byte *ByteArray::allocate(uint count) {
    if (!enoughSpace(count)) {
        uint newCapacity = ceilToPowerOf2(std::max(initialCapacity, _size + count));

        byte *newData = (byte *)realloc(_data, newCapacity);

        if (!newData)
            return 0;

        _capacity = newCapacity;
        _data = newData;
    }

    _size += count;

    return _data + _size - count;
}

int ByteArray::reallocate() {
    byte *newData = (byte *)malloc(_capacity);

    if (!newData)
        return 0;

    memcpy(newData, _data, _size);

    int delta = newData - _data;

    ::free(_data);
    _data = newData;

    return delta;
}

void ByteArray::push(const byte *data, uint size) {
    memcpy(allocate(size), data, size);
}

byte &ByteArray::operator[](int index) {
    return _data[index];
}

bool ByteArray::free(uint count) {
    if (_size < count)
        return false;

    _size -= count;
    return true;
}

void ByteArray::release() {
    ::free(_data);

    _size = 0;
    _capacity = 0;
    _data = 0;
}

bool ByteArray::enoughSpace(uint count) const {
    return _size + count <= _capacity;
}

byte *ByteArray::data() const {
    return _data;
}

uint ByteArray::size() const {
    return _size;
}

uint ByteArray::capacity() const {
    return _capacity;
}

void ByteArray::write(const std::string &fileName) const {
    std::ofstream stream(fileName, std::ios::binary);
    stream.write((char *)_data, _size);
    stream.close();
}
