#include <x86_64_compiler/compiler.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

void testCommands() try
{
    using namespace x86_64;

    Compiler c;
    std::ofstream txt("dump.txt");
    std::ofstream bin("dump.bin", std::ios::binary);
    std::stringstream s;
    std::size_t correct_count = 0;

#define X(command, correct)                                              \
    {                                                                    \
        command;                                                         \
        const auto &code = c.getCode();                                  \
        code.write(bin);                                                 \
        s << code;                                                       \
        std::string space(std::max(0, 50 - int{strlen(#command)}), ' '); \
        if (s.str() == correct)                                          \
        {                                                                \
            correct_count++;                                             \
            space[space.size() - 2] = '+';                               \
        }                                                                \
        txt << #command << space << s.str()                              \
            << (s.str() == correct ? "" : std::string(" ~ ") + correct)  \
            << std::endl;                                                \
        c.reset();                                                       \
        s.str("");                                                       \
        s.clear();                                                       \
    }

    // #include "commands.txt"

    bin << std::flush;

    system("objdump -D -w -b binary -m i386:x86-64 dump.bin > disasm.txt");

    std::cout << correct_count << std::endl;
}
catch (const std::exception &e)
{
    std::cout << "error: " << e.what() << std::endl;
}

int main()
{
    using namespace x86_64;

    Compiler c;

    c.push(RBP);
    c.mov(RSP, RBP);
    c.mov(EDI, c.mem(RBP) - 4);
    c.mov(ESI, c.mem(RBP) - 8);
    c.mov(c.mem(RBP) - 4, EDX);
    c.mov(c.mem(RBP) - 8, EAX);
    c.add(EDX, EAX);
    c.pop(RBP);
    c.ret();

    std::ofstream s("dump.bin", std::ios::binary);
    c.getCode().write(s);
    s << std::flush;
    system("objdump -D -w -b binary -m i386:x86-64 dump.bin > disasm.txt");

    Function f(c.getCode());
    std::cout << f.invoke<int>(4, 5) << std::endl;

    return 0;
}
