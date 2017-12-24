#include <x86_64_compiler/compiler.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

int main() try
{
    using namespace x86_64;

    Compiler c;
    std::ofstream txt("dump.txt");
    std::ofstream bin("dump.bin", std::ios::binary);

#define X(command, correct)                                                    \
    {                                                                          \
        command;                                                               \
        const auto &code = c.getCode();                                        \
        code.write(bin);                                                       \
        std::stringstream tmp;                                                 \
        tmp << code;                                                           \
        txt << #command                                                        \
            << std::string(std::max(0, 50 - int{strlen(#command)}), ' ')       \
            << tmp.str() << (tmp.str() == correct ? " +" : " -") << std::endl; \
        c.reset();                                                             \
    }

#include "commands.txt"

    txt << std::flush;
    bin << std::flush;

    system("objdump -D -b binary -m i386:x86-64 dump.bin | tee disasm.txt");

    return 0;
}
catch (const std::exception &e)
{
    std::cout << "error: " << e.what() << std::endl;
}
