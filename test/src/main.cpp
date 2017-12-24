#include <x86_64_compiler/compiler.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

constexpr int c_spacing = 50;

#define X(command)                                                             \
    command;                                                                   \
    s << #command                                                              \
      << std::string(                                                          \
             static_cast<std::size_t>(                                         \
                 std::max(0, c_spacing - static_cast<int>(strlen(#command)))), \
             ' ')                                                              \
      << c.getCode() << std::endl;                                             \
    c.reset();

} // anonymous namespace

int main() try
{
    using namespace x86_64;

    Compiler c;
    std::stringstream s;

#include "commands.txt"

    std::cout << s.str();

    std::ofstream fs("dump.txt");
    fs << s.str();

    return 0;
}
catch (const std::exception &e)
{
    std::cout << "error: " << e.what() << std::endl;
}
