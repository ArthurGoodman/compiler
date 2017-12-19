#pragma once

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

namespace x86_64 {

class ByteArray final {
public:
    explicit ByteArray();
    explicit ByteArray(const ByteArray &array);
    explicit ByteArray(ByteArray &&array);

    ByteArray &operator=(const ByteArray &array);
    ByteArray &operator=(ByteArray &&array);

    const uint8_t *data() const;
    uint8_t *data();

    std::size_t size() const;

    void push(const uint8_t *data, std::size_t size);

    template <class T>
    void push(const T &value);

    friend std::ostream &operator<<(
        std::ostream &stream,
        const ByteArray &array);

private:
    std::vector<uint8_t> _data;
};

template <class T>
inline void ByteArray::push(const T &value) {
    push(reinterpret_cast<const uint8_t *>(&value), sizeof(T));
}

template <>
inline void ByteArray::push<ByteArray>(const ByteArray &array) {
    push(array.data(), array.size());
}

} // namespace x86_64
