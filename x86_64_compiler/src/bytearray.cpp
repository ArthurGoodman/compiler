#include "x86_64_compiler/bytearray.hpp"

#include <iomanip>

namespace x86_64 {

ByteArray::ByteArray() {
}

ByteArray::ByteArray(const ByteArray &array)
    : _data{array._data} {
}

ByteArray::ByteArray(ByteArray &&array)
    : _data{std::move(array._data)} {
}

ByteArray &ByteArray::operator=(const ByteArray &array) {
    _data = array._data;
    return *this;
}

ByteArray &ByteArray::operator=(ByteArray &&array) {
    _data = std::move(array._data);
    return *this;
}

const uint8_t *ByteArray::data() const {
    return _data.data();
}

uint8_t *ByteArray::data() {
    return _data.data();
}

std::size_t ByteArray::size() const {
    return _data.size();
}

void ByteArray::push(const uint8_t *data, std::size_t size) {
    if (size > 0)
        _data.insert(_data.end(), data, data + size);
}

std::ostream &operator<<(std::ostream &stream, const ByteArray &array) {
    bool first = true;

    std::ios state(nullptr);
    state.copyfmt(stream);

    for (const uint8_t &value : array._data) {
        if (!first)
            stream << " ";

        first = false;

        stream << std::setfill('0') << std::setw(2) << std::hex << int{value};
    }

    stream.copyfmt(state);

    return stream;
}

} // namespace x86_64
