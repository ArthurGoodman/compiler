#include "addtest.h"

#include <iostream>

AddTest::AddTest()
    : TestSet("add") {
    $("add ebx,ebx", "01 db", [=]() { c.add(vm::EBX, vm::EBX); dump(); });
    $("add ebx,DWORD PTR [ebx]", "03 1b", [=]() { c.add(vm::EBX, vm::ptr(vm::EBX)); dump(); });
    $("add ebx,DWORD PTR [ebx+0x1]", "03 5b 01", [=]() { c.add(vm::EBX, vm::EBX  +(byte)0x1); dump(); });
    $("add ebx,DWORD PTR [ebx+0x100]", "03 9b 00 01 00 00", [=]() { c.add(vm::EBX, vm::EBX + 0x100); dump(); });
    $("add ebx,DWORD PTR [ebx+ebx*2]", "03 1c 5b", [=]() { c.add(vm::EBX, vm::EBX + vm::EBX * 2); dump(); });
    $("add ebx,DWORD PTR [ebx+ebx*2+0x1]", "03 5c 5b 01", [=]() { c.add(vm::EBX, vm::EBX + vm::EBX * 2 + (byte)0x1); dump(); });
    $("add ebx,DWORD PTR ds:0x1", "03 1d 01 00 00 00", [=]() { c.add(vm::EBX, vm::ptr(0x1)); dump(); });
    $("add ebx,0x1", "83 c3 01", [=]() { c.add(vm::EBX, (byte)0x1); dump(); });
    $("add ebx,0x100", "81 c3 00 01 00 00", [=]() { c.add(vm::EBX, 0x100); dump(); });
    $("add DWORD PTR [ebx],ebx", "01 1b", [=]() { c.add(vm::ptr(vm::EBX), vm::EBX); dump(); });
    $("add DWORD PTR [ebx+0x1],ebx", "01 5b 01", [=]() { c.add(vm::EBX + (byte)0x1, vm::EBX); dump(); });
    $("add DWORD PTR [ebx+0x100],ebx", "01 9b 00 01 00 00", [=]() { c.add(vm::EBX + 0x100, vm::EBX); dump(); });
    $("add DWORD PTR [ebx+ebx*2],ebx", "01 1c 5b", [=]() { c.add(vm::EBX + vm::EBX * 2, vm::EBX); dump(); });
    $("add DWORD PTR [ebx+ebx*2+0x1],ebx", "01 5c 5b 01", [=]() { c.add(vm::EBX + vm::EBX * 2 + (byte)0x1, vm::EBX); dump(); });
    $("add DWORD PTR ds:0x1,ebx", "01 1d 01 00 00 00", [=]() { c.add(vm::ptr(0x1), vm::EBX); dump(); });
    $("add DWORD PTR [ebx],0x1", "83 03 01", [=]() { c.add(vm::ptr(vm::EBX), (byte)0x1); dump(); });
    $("add DWORD PTR [ebx+0x1],0x1", "83 43 01 01", [=]() { c.add(vm::EBX + (byte)0x1, (byte)0x1); dump(); });
    $("add DWORD PTR [ebx+0x100],0x1", "83 83 00 01 00 00 01", [=]() { c.add(vm::EBX + 0x100, (byte)0x1); dump(); });
    $("add DWORD PTR [ebx+ebx*2],0x1", "83 04 5b 01", [=]() { c.add(vm::EBX + vm::EBX * 2, (byte)0x1); dump(); });
    $("add DWORD PTR [ebx+ebx*2+0x1],0x1", "83 44 5b 01 01", [=]() { c.add(vm::EBX + vm::EBX * 2 + (byte)0x1, (byte)0x1); dump(); });
    $("add DWORD PTR [ebx],0x100", "81 03 00 01 00 00", [=]() { c.add(vm::ptr(vm::EBX), 0x100); dump(); });
    $("add DWORD PTR [ebx+0x1],0x100", "81 43 01 00 01 00 00", [=]() { c.add(vm::EBX + (byte)0x1, 0x100); dump(); });
    $("add DWORD PTR [ebx+0x100],0x100", "81 83 00 01 00 00 00 01 00 00", [=]() { c.add(vm::EBX + 0x100, 0x100); dump(); });
    $("add DWORD PTR [ebx+ebx*2],0x100", "81 04 5b 00 01 00 00", [=]() { c.add(vm::EBX + vm::EBX * 2, 0x100); dump(); });
    $("add DWORD PTR [ebx+ebx*2+0x1],0x100", "81 44 5b 01 00 01 00 00", [=]()  { c.add(vm::EBX + vm::EBX * 2 + (byte)0x1, 0x100); dump(); });
}

void AddTest::dump() {
    std::cout << c.compile().dump();
}
