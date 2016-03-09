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
    bool free(uint count);
    void release();

    bool enoughSpace(uint count) const;

    int reallocate();

    byte *getData() const;
    uint getSize() const;
    uint getCapacity() const;

    byte &operator[](int index);
};
