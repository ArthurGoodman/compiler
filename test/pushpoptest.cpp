#include "pushpoptest.h"

#include <iostream>

PushPopTest::PushPopTest()
    : TestSet("push/pop") {
    $("push DWORD PTR [ebx]", "ff 33", [=]() { c.push(vm::ptr(vm::EBX)); dump(); });
    $("push DWORD PTR [ebx+0x1]", "ff 73 01", [=]() { c.push(vm::EBX + (byte)0x1); dump(); });
    $("push DWORD PTR [ebx+0x100]", "ff b3 00 01 00 00", [=]() { c.push(vm::EBX + 0x100); dump(); });
    $("push DWORD PTR [ebx+ebx*2+0x1]", "ff 74 5b 01", [=]() { c.push(vm::EBX + vm::EBX * 2 + (byte)0x1); dump(); });
    $("push DWORD PTR [ebx+ebx*2+0x100]", "ff b4 5b 00 01 00 00", [=]() { c.push(vm::EBX + vm::EBX * 2 + 0x100); dump(); });
    $("push DWORD PTR ds:0x1", "ff 35 01 00 00 00", [=]() { c.push(vm::ptr(0x1)); dump(); });
    $("push DWORD PTR ds:0x100", "ff 35 00 01 00 00", [=]() { c.push(vm::ptr(0x100)); dump(); });
    $("push ebx", "53", [=]() { c.push(vm::EBX); dump(); });
    $("push 0x1", "6a 01", [=]() { c.push((byte)0x1); dump(); });
    $("push 0x100", "68 00 01 00 00", [=]() { c.push(0x100); dump(); });

    $("pop DWORD PTR [ebx]", "8f 03", [=]() { c.pop(vm::ptr(vm::EBX)); dump(); });
    $("pop DWORD PTR [ebx+0x1]", "8f 43 01", [=]() { c.pop(vm::EBX + (byte)0x1); dump(); });
    $("pop DWORD PTR [ebx+0x100]", "8f 83 00 01 00 00", [=]() { c.pop(vm::EBX + 0x100); dump(); });
    $("pop DWORD PTR [ebx+ebx*2+0x1]", "8f 44 5b 01", [=]() { c.pop(vm::EBX + vm::EBX * 2 + (byte)0x1); dump(); });
    $("pop DWORD PTR [ebx+ebx*2+0x100]", "8f 84 5b 00 01 00 00", [=]() { c.pop(vm::EBX + vm::EBX * 2 + 0x100); dump(); });
    $("pop DWORD PTR ds:0x1", "8f 05 01 00 00 00", [=]() { c.pop(vm::ptr(0x1)); dump(); });
    $("pop DWORD PTR ds:0x100", "8f 05 00 01 00 00", [=]() { c.pop(vm::ptr(0x100)); dump(); });
    $("pop ebx", "5b", [=]() { c.pop(vm::EBX); dump(); });
}

void PushPopTest::dump() {
    std::cout << c.compile().dump();
}
