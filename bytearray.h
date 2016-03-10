#pragma once

#include "common.h"

class ByteArray {
    static uint initialCapacity;

    uint size, capacity;
    byte *data;

public:
    static void setInitialCapacity(uint initialCapacity);

    ByteArray();

    ByteArray(const ByteArray &array);
    ByteArray(ByteArray &&array);

    ~ByteArray();

    ByteArray &operator=(const ByteArray &array);
    ByteArray &operator=(ByteArray &&array);

    byte *allocate(uint count);
    int reallocate();

    template <class T>
    ByteArray &push(T value);

    template <class T>
    T pop();

    byte &operator[](int index);

    bool free(uint count);
    void release();

    bool enoughSpace(uint count) const;

    byte *getData() const;
    uint getSize() const;
    uint getCapacity() const;
};

template <class T>
ByteArray &ByteArray::push(T value) {
    *(T *)allocate(sizeof(T)) = value;
    return *this;
}

template <class T>
T ByteArray::pop() {
    free(sizeof(T));
    return *(T *)(data + size);
}
