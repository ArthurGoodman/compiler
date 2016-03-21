#include "pushpoptest.h"

PushPopTest::PushPopTest()
    : CompilerTest("push/pop") {
    $("push DWORD PTR [ebx]", "ff 33", [=]() {
        c.push(vm::ptr(vm::EBX));
    });

    $("push DWORD PTR [ebx+0x1]", "ff 73 01", [=]() {
        c.push(vm::EBX + (byte)0x1);
    });

    $("push DWORD PTR [ebx+0x100]", "ff b3 00 01 00 00", [=]() {
        c.push(vm::EBX + 0x100);
    });

    $("push DWORD PTR [ebx+ebx*2+0x1]", "ff 74 5b 01", [=]() {
        c.push(vm::EBX + vm::EBX * 2 + (byte)0x1);
    });

    $("push DWORD PTR [ebx+ebx*2+0x100]", "ff b4 5b 00 01 00 00", [=]() {
        c.push(vm::EBX + vm::EBX * 2 + 0x100);
    });

    $("push DWORD PTR ds:0x1", "ff 35 01 00 00 00", [=]() {
        c.push(vm::ptr(0x1));
    });

    $("push DWORD PTR ds:0x100", "ff 35 00 01 00 00", [=]() {
        c.push(vm::ptr(0x100));
    });

    $("push ebx", "53", [=]() {
        c.push(vm::EBX);
    });

    $("push 0x1", "6a 01", [=]() {
        c.push((byte)0x1);
    });

    $("push 0x100", "68 00 01 00 00", [=]() {
        c.push(0x100);
    });

    $("pop DWORD PTR [ebx]", "8f 03", [=]() {
        c.pop(vm::ptr(vm::EBX));
    });

    $("pop DWORD PTR [ebx+0x1]", "8f 43 01", [=]() {
        c.pop(vm::EBX + (byte)0x1);
    });

    $("pop DWORD PTR [ebx+0x100]", "8f 83 00 01 00 00", [=]() {
        c.pop(vm::EBX + 0x100);
    });

    $("pop DWORD PTR [ebx+ebx*2+0x1]", "8f 44 5b 01", [=]() {
        c.pop(vm::EBX + vm::EBX * 2 + (byte)0x1);
    });

    $("pop DWORD PTR [ebx+ebx*2+0x100]", "8f 84 5b 00 01 00 00", [=]() {
        c.pop(vm::EBX + vm::EBX * 2 + 0x100);
    });

    $("pop DWORD PTR ds:0x1", "8f 05 01 00 00 00", [=]() {
        c.pop(vm::ptr(0x1));
    });

    $("pop DWORD PTR ds:0x100", "8f 05 00 01 00 00", [=]() {
        c.pop(vm::ptr(0x100));
    });

    $("pop ebx", "5b", [=]() {
        c.pop(vm::EBX);
    });
}
