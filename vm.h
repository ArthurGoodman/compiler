#pragma once

namespace vm {
enum Register {
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

enum Mod {
    Disp0,
    Disp8,
    Disp32,
    Reg
};
}
