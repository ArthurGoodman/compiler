#include "x86_64_compiler/bytearray.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace x86_64 {

ByteArray::ByteArray()
{
}

ByteArray::ByteArray(const ByteArray &array)
    : _data{array._data}
{
}

ByteArray::ByteArray(ByteArray &&array)
    : _data{std::move(array._data)}
{
}

ByteArray &ByteArray::operator=(const ByteArray &array)
{
    _data = array._data;
    return *this;
}

ByteArray &ByteArray::operator=(ByteArray &&array)
{
    _data = std::move(array._data);
    return *this;
}

uint8_t *ByteArray::push(const uint8_t *data, std::size_t size)
{
    if (size > 0)
    {
        if (data)
            _data.insert(_data.end(), data, data + size);
        else
            _data.insert(_data.end(), size, 0);
    }

    return back(size);
}

void ByteArray::pop(std::size_t size)
{
    _data.erase(
        _data.end() - static_cast<int32_t>(std::min(size, _data.size())),
        _data.end());
}

uint8_t ByteArray::operator[](std::size_t index) const
{
    return _data[index];
}

uint8_t &ByteArray::operator[](std::size_t index)
{
    return _data[index];
}

const uint8_t *ByteArray::data() const
{
    return _data.data();
}

uint8_t *ByteArray::data()
{
    return _data.data();
}

std::size_t ByteArray::size() const
{
    return _data.size();
}

std::size_t ByteArray::capacity() const
{
    return _data.capacity();
}

void ByteArray::write(const std::string &file_name) const
{
    std::ofstream stream(file_name, std::ios::binary);
    stream.write(
        reinterpret_cast<const char *>(_data.data()),
        static_cast<std::streamsize>(size()));
}

std::ostream &operator<<(std::ostream &stream, const ByteArray &array)
{
    bool first = true;

    std::ios state(nullptr);
    state.copyfmt(stream);

    for (const uint8_t &value : array._data)
    {
        if (!first)
            stream << " ";

        first = false;

        stream << std::setfill('0') << std::setw(2) << std::hex << int{value};
    }

    stream.copyfmt(state);

    return stream;
}

} // namespace x86_64
