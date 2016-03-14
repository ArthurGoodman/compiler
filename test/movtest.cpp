#include "movtest.h"

#include <iostream>

MovTest::MovTest()
    : TestSet("mov") {
    $("mov ebx,ebx", "89 db", [=]() { c.mov(vm::EBX, vm::EBX); dump(); });
    $("mov ebx,DWORD PTR [ebx]", "8b 1b", [=]() { c.mov(vm::EBX, vm::ptr(vm::EBX)); dump(); });
    $("mov ebx,DWORD PTR [ebx+0x1]", "8b 5b 01", [=]() { c.mov(vm::EBX, vm::EBX + (byte)0x1); dump(); });
    $("mov ebx,DWORD PTR [ebx+0x100]", "8b 9b 00 01 00 00", [=]() { c.mov(vm::EBX, vm::EBX + 0x100); dump(); });
    $("mov ebx,DWORD PTR [ebx+ebx*2]", "8b 1c 5b", [=]() { c.mov(vm::EBX, vm::EBX + vm::EBX * 2); dump(); });
    $("mov ebx,DWORD PTR [ebx+ebx*2+0x1]", "8b 5c 5b 01", [=]() { c.mov(vm::EBX, vm::EBX + vm::EBX * 2 + (byte)0x1); dump(); });
    $("mov ebx,DWORD PTR ds:0x1", "8b 1d 01 00 00 00", [=]() { c.mov(vm::EBX, vm::ptr(0x1)); dump(); });
    $("mov ebx,0x1", "bb 01 00 00 00", [=]() { c.mov(vm::EBX, 0x1); dump(); });
    $("mov ebx,0x100", "bb 00 01 00 00", [=]() { c.mov(vm::EBX, 0x100); dump(); });
    $("mov DWORD PTR [ebx],ebx", "89 1b", [=]() { c.mov(vm::ptr(vm::EBX), vm::EBX); dump(); });
    $("mov DWORD PTR [ebx+0x1],ebx", "89 5b 01", [=]() { c.mov(vm::EBX + (byte)0x1, vm::EBX); dump(); });
    $("mov DWORD PTR [ebx+0x100],ebx", "89 9b 00 01 00 00", [=]() { c.mov(vm::EBX + 0x100, vm::EBX); dump(); });
    $("mov DWORD PTR [ebx+ebx*2],ebx", "89 1c 5b", [=]() { c.mov(vm::EBX + vm::EBX * 2, vm::EBX); dump(); });
    $("mov DWORD PTR [ebx+ebx*2+0x1],ebx", "89 5c 5b 01", [=]() { c.mov(vm::EBX + vm::EBX * 2 + (byte)0x1, vm::EBX); dump(); });
    $("mov DWORD PTR ds:0x1,ebx", "89 1d 01 00 00 00", [=]() { c.mov(vm::ptr(0x1), vm::EBX); dump(); });
    $("mov DWORD PTR [ebx],0x1", "c7 03 01 00 00 00", [=]() { c.mov(vm::ptr(vm::EBX), 0x1); dump(); });
    $("mov DWORD PTR [ebx+0x1],0x1", "c7 43 01 01 00 00 00", [=]() { c.mov(vm::EBX + (byte)0x1, 0x1); dump(); });
    $("mov DWORD PTR [ebx+0x100],0x1", "c7 83 00 01 00 00 01 00 00 00", [=]() { c.mov(vm::EBX + 0x100, 0x1); dump(); });
    $("mov DWORD PTR [ebx+ebx*2],0x1", "c7 04 5b 01 00 00 00", [=]() { c.mov(vm::EBX + vm::EBX * 2, 0x1); dump(); });
    $("mov DWORD PTR [ebx+ebx*2+0x1],0x1", "c7 44 5b 01 01 00 00 00", [=]() { c.mov(vm::EBX + vm::EBX * 2 + (byte)0x1, 0x1); dump(); });
    $("mov DWORD PTR [ebx],0x100", "c7 03 00 01 00 00", [=]() { c.mov(vm::ptr(vm::EBX), 0x100); dump(); });
    $("mov DWORD PTR [ebx+0x1],0x100", "c7 43 01 00 01 00 00", [=]() { c.mov(vm::EBX + (byte)0x1, 0x100); dump(); });
    $("mov DWORD PTR [ebx+0x100],0x100", "c7 83 00 01 00 00 00 01 00 00", [=]() { c.mov(vm::EBX + 0x100, 0x100); dump(); });
    $("mov DWORD PTR [ebx+ebx*2],0x100", "c7 04 5b 00 01 00 00", [=]() { c.mov(vm::EBX + vm::EBX * 2, 0x100); dump(); });
    $("mov DWORD PTR [ebx+ebx*2+0x1],0x100", "c7 44 5b 01 00 01 00 00", [=]() { c.mov(vm::EBX + vm::EBX * 2 + (byte)0x1, 0x100); dump(); });
}

void MovTest::dump() {
    std::cout << c.compile().dump();
}
