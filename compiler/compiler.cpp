#include "compiler.h"

#include "common.h"

#include <cassert>
#include <cmath>

namespace x86 {

void Compiler::rdata(const std::string &name, const byte *data, uint size) {
    uint offset = section(RDATA).size();
    section(RDATA).push(data, size);
    pushSymbol(name, ".rdata", offset);
}

void Compiler::data(const std::string &name, const byte *data, uint size) {
    uint offset = section(DATA).size();
    section(DATA).push(data, size);
    pushSymbol(name, ".data", offset);
}

void Compiler::bss(const std::string &name, uint size) {
    uint offset = section(BSS).size();
    section(BSS).allocate(size);
    pushSymbol(name, ".bss", offset);
}

void Compiler::external(const std::string &name) {
    pushSymbol(name, "_" + name, 0);
}

void Compiler::function(const std::string &name) {
    pushSymbol(name, ".text", section(TEXT).size());
}

void Compiler::push(const MemRef &ref) {
    if (ref.isAddress())
        regRMInstruction(0xff, ref, ESI);
    else
        gen((byte)(0x50 + ref));
}

void Compiler::push(byte value) {
    gen((byte)0x6a);
    gen(value);
}

void Compiler::push(int value) {
    gen((byte)0x68);
    gen(value);
}

void Compiler::push(Register reg) {
    push(MemRef(reg));
}

void Compiler::pop(const MemRef &ref) {
    if (ref.isAddress())
        regRMInstruction(0x8f, ref, EAX);
    else
        gen((byte)(0x58 + ref));
}

void Compiler::mov(const MemRef &dst, const MemRef &src) {
    regRMInstruction(0x89, dst, src);
}

void Compiler::mov(const MemRef &ref, byte value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        gen((byte)(0xb0 + ref));

    gen(value);
}

void Compiler::mov(const MemRef &ref, int value) {
    if (ref.isAddress())
        regRMInstruction(0xc7, ref, EAX);
    else
        gen((byte)(0xb8 + ref));

    gen(value);
}

void Compiler::mov(const MemRef &dst, Register src) {
    mov(dst, MemRef(src));
}

void Compiler::lea(const MemRef &dst, const MemRef &src) {
    assert(src.isAddress() && "src should be an address");

    regRMInstruction(0x8d - 0x2, dst, src);
}

void Compiler::add(const MemRef &op1, const MemRef &op2) {
    regRMInstruction(0x1, op1, op2);
}

void Compiler::add(const MemRef &ref, byte value) {
    regRMInstruction(0x83, ref, EAX);
    gen(value);
}

void Compiler::add(const MemRef &ref, int value) {
    regRMInstruction(0x81, ref, EAX);
    gen(value);
}

void Compiler::add(const MemRef &op1, Register op2) {
    add(op1, MemRef(op2));
}

void Compiler::sub(const MemRef &op1, const MemRef &op2) {
    regRMInstruction(0x29, op1, op2);
}

void Compiler::sub(const MemRef &ref, byte value) {
    regRMInstruction(0x83, ref, EBP);
    gen(value);
}

void Compiler::sub(const MemRef &ref, int value) {
    regRMInstruction(0x83, ref, EBP);
    gen(value);
}

void Compiler::sub(const MemRef &op1, Register op2) {
    sub(op1, MemRef(op2));
}

void Compiler::leave() {
    gen((byte)0xc9);
}

void Compiler::ret() {
    gen((byte)0xc3);
}

void Compiler::nop() {
    gen((byte)0x90);
}

//Function Compiler::compile() {
//    return std::move(f);
//}

ByteArray Compiler::writeOBJ() const {
    ByteArray image;

    FileHeader fileHeader = {};

    fileHeader.machine = IMAGE_FILE_MACHINE_I386;
    fileHeader.characteristics = IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_LINE_NUMS_STRIPPED;

    std::map<SectionID, SectionHeader> sectionHeaders;

    if (isSectionDefined(TEXT)) {
        fileHeader.numberOfSections++;

        SectionHeader header = {};

        strcat(header.name, ".text");
        header.sizeOfRawData = section(TEXT).size();
        header.pointerToRawData = 0;
        header.pointerToRelocations = 0;
        header.numberOfRelocations = 0;
        header.characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE;

        sectionHeaders[TEXT] = header;
    }

    if (isSectionDefined(RDATA)) {
        fileHeader.numberOfSections++;

        SectionHeader header = {};

        strcat(header.name, ".rdata");
        header.sizeOfRawData = section(RDATA).size();
        header.pointerToRawData = 0;
        header.characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA;

        sectionHeaders[RDATA] = header;
    }

    if (isSectionDefined(DATA)) {
        fileHeader.numberOfSections++;

        SectionHeader header = {};

        strcat(header.name, ".data");
        header.sizeOfRawData = section(DATA).size();
        header.pointerToRawData = 0;
        header.characteristics = IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA;

        sectionHeaders[DATA] = header;
    }

    if (isSectionDefined(BSS)) {
        fileHeader.numberOfSections++;

        SectionHeader header = {};

        strcat(header.name, ".bss");
        header.sizeOfRawData = section(BSS).size();
        header.pointerToRawData = 0;
        header.characteristics = IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_UNINITIALIZED_DATA;

        sectionHeaders[BSS] = header;
    }

    std::vector<SymbolTableEntry> symbolTable;

    // ...

    image.push(fileHeader);

    for (auto &sectionHeader : sectionHeaders) {
        sectionHeader.second.pointerToRawData = 0;
        image.push(sectionHeader.second);
    }

    for (auto &section : sections)
        image.push(section.second);

    return image;
}

ByteArray Compiler::writeEXE() const {
    ByteArray image;
    return image;
}

ByteArray Compiler::writeDLL(const std::string & /*name*/) const {
    ByteArray image;
    return image;
}

void Compiler::regRMInstruction(byte op, const MemRef &op1, const MemRef &op2) {
    assert(!(op1.isAddress() && op2.isAddress()) && "too many memory references");

    gen((byte)(op + (op2.isAddress() ? 0x2 : 0x0)));

    if (!op1.isAddress() && !op2.isAddress()) {
        modRegRM(Reg, op2, op1);
        return;
    }

    const MemRef &r = op1.isAddress() ? op2 : op1;
    const MemRef &rm = op1.isAddress() ? op1 : op2;

    switch (rm.getDispSize()) {
    case 0:
        if (rm.getScale() != 0) {
            modRegRM(Disp0, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp0, r, rm);
        break;

    case 1:
        if (rm.getScale() != 0) {
            modRegRM(Disp8, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp8, r, rm);
        gen((byte)rm.getDisp());
        break;

    case 4:
        if (rm.getBase() == NOREG)
            modRegRM(Disp0, r, EBP);
        else if (rm.getScale() != 0) {
            modRegRM(Disp32, r, ESP);
            modRegRM(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            modRegRM(Disp32, r, rm);
        gen(rm.getDisp());
        break;

    default:
        break;
    }
}

void Compiler::modRegRM(byte mod, byte reg, byte rm) {
    gen((byte)(mod << 6 | reg << 3 | rm));
}

bool Compiler::isSectionDefined(Compiler::SectionID id) const {
    return sections.find(id) != sections.end();
}

ByteArray &Compiler::section(SectionID id) {
    if (!isSectionDefined(id))
        sections[id] = ByteArray();

    return sections.at(id);
}

const ByteArray &Compiler::section(Compiler::SectionID id) const {
    return sections.at(id);
}

void Compiler::pushSymbol(const std::string &name, const std::string &baseSymbol, uint offset) {
    symbols << Symbol{name, baseSymbol, offset};
}

//const char *Compiler::sectionIDToName(SectionID id) {
//    switch (id) {
//    case TEXT:
//        return ".text";

//    case EDATA:
//        return ".edata";

//    case IDATA:
//        return ".idata";

//    case RDATA:
//        return ".rdata";

//    case DATA:
//        return ".data";

//    case BSS:
//        return ".bss";

//    case RELOC:
//        return ".reloc";

//    default:
//        return 0;
//    }
//}
}
