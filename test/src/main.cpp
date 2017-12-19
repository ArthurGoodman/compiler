#include <x86_64_compiler/bytearray.hpp>

#include <iostream>

int main() {
    x86_64::ByteArray array;

    array.push(123);
    array.push(456);
    array.push(789);

    std::cout << array << std::endl;
    std::cout << array.size() << std::endl;

    return 0;
}
