#include <iostream>

#include "compiler.h"

int main() {
    x86::Compiler c;

    //    c.rdata("string", "Hello, World!");

    //    c.externalFunction("puts");

    //    c.function("f");

    //    c.push(x86::EBP);
    //    c.mov(x86::EBP, x86::ESP);
    //    c.push(c.abs("string"));
    //    c.call(c.rel("puts"));
    //    c.leave();
    //    c.ret();

    c.push(c.ref(x86::EAX));
    c.push(c.ref(x86::EBX));
    c.push(c.ref(x86::ECX));
    c.push(c.ref(x86::EDX));
    c.push(c.ref(x86::EBP));
    c.push(c.ref(x86::ESP));
    c.push(c.ref(x86::EDI));
    c.push(c.ref(x86::ESI));
    c.push(c.ref((byte)0x10, x86::EAX));
    c.push(c.ref((byte)0x10, x86::EBX));
    c.push(c.ref((byte)0x10, x86::ECX));
    c.push(c.ref((byte)0x10, x86::EDX));
    c.push(c.ref((byte)0x10, x86::EBP));
    c.push(c.ref((byte)0x10, x86::ESP));
    c.push(c.ref((byte)0x10, x86::EDI));
    c.push(c.ref((byte)0x10, x86::ESI));
    c.push(c.ref(0x100, x86::EAX));
    c.push(c.ref(0x100, x86::EBX));
    c.push(c.ref(0x100, x86::ECX));
    c.push(c.ref(0x100, x86::EDX));
    c.push(c.ref(0x100, x86::EBP));
    c.push(c.ref(0x100, x86::ESP));
    c.push(c.ref(0x100, x86::EDI));
    c.push(c.ref(0x100, x86::ESI));
    c.push(c.ref(0x100));
    c.push(c.ref(x86::EBX, x86::EDX, 8));
    c.push(c.ref(x86::EBX, x86::EDX, 4));
    c.push(c.ref(x86::EBX, x86::EDX, 2));
    c.push(c.ref(x86::EBX, x86::EDX, 1));
    c.push(c.ref((byte)0x10, x86::EBX, x86::EDX, 8));
    c.push(c.ref((byte)0x10, x86::EBX, x86::EDX, 4));
    c.push(c.ref((byte)0x10, x86::EBX, x86::EDX, 2));
    c.push(c.ref((byte)0x10, x86::EBX, x86::EDX, 1));
    c.push(c.ref(0x100, x86::EBX, x86::EDX, 8));
    c.push(c.ref(0x100, x86::EBX, x86::EDX, 4));
    c.push(c.ref(0x100, x86::EBX, x86::EDX, 2));
    c.push(c.ref(0x10, x86::EBX, x86::EDX, 1));
    c.push(c.ref(0x10, x86::EDX, 1));
    c.push(c.ref(0x100, x86::EDX, 1));
    c.push(c.ref(x86::EDX, 1));

    c.push(c.ref(x86::EAX, x86::EAX, 1));
    c.push(c.ref(x86::EBX, x86::EAX, 1));
    c.push(c.ref(x86::ECX, x86::EAX, 1));
    c.push(c.ref(x86::EDX, x86::EAX, 1));
    c.push(c.ref(x86::ESP, x86::EAX, 1));
    c.push(c.ref(x86::EBP, x86::EAX, 1));
    c.push(c.ref(x86::ESI, x86::EAX, 1));
    c.push(c.ref(x86::EDI, x86::EAX, 1));
    c.push(c.ref(x86::EAX, x86::EBX, 1));
    c.push(c.ref(x86::EBX, x86::EBX, 1));
    c.push(c.ref(x86::ECX, x86::EBX, 1));
    c.push(c.ref(x86::EDX, x86::EBX, 1));
    c.push(c.ref(x86::ESP, x86::EBX, 1));
    c.push(c.ref(x86::EBP, x86::EBX, 1));
    c.push(c.ref(x86::ESI, x86::EBX, 1));
    c.push(c.ref(x86::EDI, x86::EBX, 1));
    c.push(c.ref(x86::EAX, x86::ECX, 1));
    c.push(c.ref(x86::EBX, x86::ECX, 1));
    c.push(c.ref(x86::ECX, x86::ECX, 1));
    c.push(c.ref(x86::EDX, x86::ECX, 1));
    c.push(c.ref(x86::ESP, x86::ECX, 1));
    c.push(c.ref(x86::EBP, x86::ECX, 1));
    c.push(c.ref(x86::ESI, x86::ECX, 1));
    c.push(c.ref(x86::EDI, x86::ECX, 1));
    c.push(c.ref(x86::EAX, x86::EDX, 1));
    c.push(c.ref(x86::EBX, x86::EDX, 1));
    c.push(c.ref(x86::ECX, x86::EDX, 1));
    c.push(c.ref(x86::EDX, x86::EDX, 1));
    c.push(c.ref(x86::ESP, x86::EDX, 1));
    c.push(c.ref(x86::EBP, x86::EDX, 1));
    c.push(c.ref(x86::ESI, x86::EDX, 1));
    c.push(c.ref(x86::EDI, x86::EDX, 1));
    c.push(c.ref(x86::EAX, x86::EBP, 1));
    c.push(c.ref(x86::EBX, x86::EBP, 1));
    c.push(c.ref(x86::ECX, x86::EBP, 1));
    c.push(c.ref(x86::EDX, x86::EBP, 1));
    c.push(c.ref(x86::ESP, x86::EBP, 1));
    c.push(c.ref(x86::EBP, x86::EBP, 1));
    c.push(c.ref(x86::ESI, x86::EBP, 1));
    c.push(c.ref(x86::EDI, x86::EBP, 1));
    c.push(c.ref(x86::EAX, x86::ESI, 1));
    c.push(c.ref(x86::EBX, x86::ESI, 1));
    c.push(c.ref(x86::ECX, x86::ESI, 1));
    c.push(c.ref(x86::EDX, x86::ESI, 1));
    c.push(c.ref(x86::ESP, x86::ESI, 1));
    c.push(c.ref(x86::EBP, x86::ESI, 1));
    c.push(c.ref(x86::ESI, x86::ESI, 1));
    c.push(c.ref(x86::EDI, x86::ESI, 1));
    c.push(c.ref(x86::EAX, x86::EDI, 1));
    c.push(c.ref(x86::EBX, x86::EDI, 1));
    c.push(c.ref(x86::ECX, x86::EDI, 1));
    c.push(c.ref(x86::EDX, x86::EDI, 1));
    c.push(c.ref(x86::ESP, x86::EDI, 1));
    c.push(c.ref(x86::EBP, x86::EDI, 1));
    c.push(c.ref(x86::ESI, x86::EDI, 1));
    c.push(c.ref(x86::EDI, x86::EDI, 1));

    c.push(x86::EAX);
    c.push(x86::EBX);
    c.push(x86::ECX);
    c.push(x86::EDX);
    c.push(x86::ESP);
    c.push(x86::EBP);
    c.push(x86::ESI);
    c.push(x86::EDI);

    c.push((byte)0x10);

    c.push(0x100);

    c.writeOBJ().write("a.o");
    system("objdump -d a.o");

    return 0;
}
