#include "compiler.h"

#include "common.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <exception>

namespace x86 {

Compiler::SymRef Compiler::SymRef::operator+(int offset) const {
    return SymRef{ name, type, this->offset + offset };
}

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

void Compiler::externalFunction(const std::string &name) {
    pushSymbol(name, "_" + name, 0);
    externFuncs << name;
}

void Compiler::externalVariable(const std::string &name) {
    pushSymbol(name, "_" + name, 0);
    externVars << name;
}

void Compiler::function(const std::string &name) {
    pushSymbol(name, ".text", section(TEXT).size());
    funcs << name;
}

Compiler::SymRef Compiler::abs(const std::string &name) const {
    return SymRef{ name, RefAbs, 0 };
}

Compiler::SymRef Compiler::rel(const std::string &name) const {
    return SymRef{ name, RefRel, 0 };
}

Compiler::MemRef Compiler::ref(Register reg) const {
    if (reg == EBP)
        return MemRef{ Disp8, reg, 0, 0, 0, 0 };
    else if (reg == ESP)
        return MemRef{ Disp0, 4, 1, reg, reg, 0 };

    return MemRef{ Disp0, reg, 0, 0, 0, 0 };
}

Compiler::MemRef Compiler::ref(byte disp, Register reg) const {
    if (reg == ESP)
        return MemRef{ Disp8, 4, 1, reg, reg, disp };

    return MemRef{ Disp8, reg, 0, 0, 0, disp };
}

Compiler::MemRef Compiler::ref(int disp, Register reg) const {
    if (reg == ESP)
        return MemRef{ Disp32, 4, 1, reg, reg, disp };

    return MemRef{ Disp32, reg, 0, 0, 0, disp };
}

Compiler::MemRef Compiler::ref(int disp) const {
    return MemRef{ Disp0, 5, 0, 0, 0, disp };
}

Compiler::MemRef Compiler::ref(Register base, Register index, byte scale) const {
    if (index == ESP)
        throw std::runtime_error("%esp cannot be an index");

    if (base == EBP)
        return MemRef{ Disp8, 4, scale, index, base, 0 };

    return MemRef{ Disp0, 4, scale, index, base, 0 };
}

Compiler::MemRef Compiler::ref(byte disp, Register base, Register index, byte scale) const {
    if (index == ESP)
        throw std::runtime_error("%esp cannot be an index");

    return MemRef{ Disp8, 4, scale, index, base, disp };
}

Compiler::MemRef Compiler::ref(int disp, Register base, Register index, byte scale) const {
    if (index == ESP)
        throw std::runtime_error("%esp cannot be an index");

    return MemRef{ Disp32, 4, scale, index, base, disp };
}

Compiler::MemRef Compiler::ref(int disp, Register index, byte scale) const {
    if (index == ESP)
        throw std::runtime_error("%esp cannot be an index");

    return MemRef{ Disp0, 4, scale, index, 5, disp };
}

Compiler::MemRef Compiler::ref(Register index, byte scale) const {
    if (index == ESP)
        throw std::runtime_error("%esp cannot be an index");

    return MemRef{ Disp0, 4, scale, index, 5, 0 };
}

void Compiler::push(Register reg) {
    gen((byte)(0x50 + reg));
}

void Compiler::push(const MemRef &ref) {
    gen((byte)0xff);
    gen(6, ref);
}

void Compiler::push(byte value) {
    gen((byte)0x6a);
    gen(value);
}

void Compiler::push(int value) {
    gen((byte)0x68);
    gen(value);
}

void Compiler::push(const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    gen((byte)0x68);

    uint offset = sectionSize(TEXT);

    gen(ref.offset);

    pushReloc(Reloc{ ref.name, ref.type, offset });
}

void Compiler::pop(Register reg) {
    gen((byte)(0x58 + reg));
}

void Compiler::pop(const MemRef &ref) {
    gen((byte)0x8f);
    gen(0, ref);
}

void Compiler::call(int disp) {
    gen((byte)0xe8);
    gen(disp);
}

void Compiler::call(const MemRef &ref) {
    gen((byte)0xff);
    gen(2, ref);
}

void Compiler::call(const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    gen((byte)0xe8);

    uint offset = sectionSize(TEXT);

    gen(ref.offset);

    pushReloc(Reloc{ ref.name, ref.type, offset });
}

void Compiler::mov(Register src, Register dst) {
    gen((byte)0x89);
    gen(src, MemRef{ Reg, dst, 0, 0, 0, 0 });
}

void Compiler::mov(int src, Register dst) {
    gen((byte)(0xb8 + dst));
    gen(src);
}

void Compiler::mov(int src, const MemRef &dst) {
    gen((byte)0xc7);
    gen(0, dst);
    gen(src);
}

void Compiler::mov(Register src, const MemRef &dst) {
    gen((byte)0x89);
    gen(src, dst);
}

void Compiler::mov(const MemRef &src, Register dst) {
    gen((byte)0x8b);
    gen(dst, src);
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

ByteArray Compiler::writeOBJ() const {
    ByteArray image;

    FileHeader fileHeader = {};

    fileHeader.machine = IMAGE_FILE_MACHINE_I386;
    fileHeader.numberOfSections = 4;
    fileHeader.characteristics = IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_LINE_NUMS_STRIPPED;

    uint ptr = sizeof(fileHeader) + fileHeader.numberOfSections * sizeof(SectionHeader);

    std::vector<SectionHeader> sectionHeaders;

    SectionHeader header = {};

    strcat(header.name, ".text");
    header.sizeOfRawData = sectionSize(TEXT);
    header.pointerToRawData = ptr;
    header.characteristics = IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_CODE;

    ptr += header.sizeOfRawData;
    sectionHeaders << header;

    header = {};

    strcat(header.name, ".data");
    header.sizeOfRawData = sectionSize(DATA);
    header.pointerToRawData = header.sizeOfRawData ? ptr : 0;
    header.characteristics = IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA;

    ptr += header.sizeOfRawData;
    sectionHeaders << header;

    header = {};

    strcat(header.name, ".bss");
    header.sizeOfRawData = sectionSize(BSS);
    header.pointerToRawData = header.sizeOfRawData ? ptr : 0;
    header.characteristics = IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_UNINITIALIZED_DATA;

    ptr += header.sizeOfRawData;
    sectionHeaders << header;

    header = {};

    strcat(header.name, ".rdata");
    header.sizeOfRawData = sectionSize(RDATA);
    header.pointerToRawData = header.sizeOfRawData ? ptr : 0;
    header.characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_CNT_INITIALIZED_DATA;

    ptr += header.sizeOfRawData;
    sectionHeaders << header;

    sectionHeaders[0].pointerToRelocations = ptr;
    sectionHeaders[0].numberOfRelocations = relocs.size();

    std::vector<RelocationDirective> textRelocs;
    std::vector<SymbolTableEntry> symbolTable;
    std::vector<std::string> symbolNames;
    uint stringTableSize = 0;
    ByteArray stringTable;

    for (const std::string &func : funcs) {
        SymbolTableEntry entry = {};

        strcat(entry.e.name, ("_" + func).data());
        entry.value = symbols.at(func).offset;
        entry.sectionNumber = TEXT;
        entry.type = IMAGE_SYM_DTYPE_FUNCTION << SCT_COMPLEX_TYPE_SHIFT;
        entry.storageClass = IMAGE_SYM_CLASS_EXTERNAL;

        symbolTable << entry;
        symbolNames << entry.e.name;
    }

    uint sectionNumber = 1;
    for (auto &header : sectionHeaders) {
        SymbolTableEntry entry = {};

        strcat(entry.e.name, header.name);
        entry.sectionNumber = sectionNumber++;
        entry.type = IMAGE_SYM_TYPE_NULL;
        entry.storageClass = IMAGE_SYM_CLASS_STATIC;

        symbolTable << entry;
        symbolNames << entry.e.name;
    }

    for (const std::string &func : externFuncs) {
        SymbolTableEntry entry = {};

        strcat(entry.e.name, ("_" + func).data());
        entry.sectionNumber = IMAGE_SYM_UNDEFINED;
        entry.type = IMAGE_SYM_DTYPE_FUNCTION << SCT_COMPLEX_TYPE_SHIFT;
        entry.storageClass = IMAGE_SYM_CLASS_EXTERNAL;

        symbolTable << entry;
        symbolNames << entry.e.name;
    }

    for (auto &reloc : relocs) {
        RelocationDirective dir = {};

        dir.virtualAddress = reloc.offset;
        dir.symbolIndex = find(symbolNames.begin(), symbolNames.end(), symbols.at(reloc.name).baseSymbol) - symbolNames.begin();
        dir.type = reloc.type == RefRel ? IMAGE_REL_I386_REL32 : IMAGE_REL_I386_DIR32;

        textRelocs << dir;
    }

    fileHeader.pointerToSymbolTable = ptr + relocs.size() * sizeof(RelocationDirective);
    fileHeader.numberOfSymbols = symbolTable.size();

    image.push(fileHeader);

    image.push((byte *)sectionHeaders.data(), sectionHeaders.size() * sizeof(SectionHeader));

    for (auto &section : sections)
        image.push(section.second);

    image.push((byte *)textRelocs.data(), textRelocs.size() * sizeof(RelocationDirective));
    image.push((byte *)symbolTable.data(), symbolTable.size() * sizeof(SymbolTableEntry));

    if (stringTableSize > 0) {
        image.push(stringTableSize);
        image.push(stringTable);
    }

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

void Compiler::gen(byte reg, const MemRef &ref) {
    composeByte(ref.mod, reg, ref.rm);

    if (ref.scale != 0)
        composeByte(log2(ref.scale), ref.index, ref.base);

    if (ref.mod == Disp8)
        gen((byte)ref.disp);
    else if (ref.mod == Disp32 || (ref.mod == Disp0 && ref.rm == 5) || (ref.rm == 4 && ref.base == 5))
        gen(ref.disp);
}

void Compiler::composeByte(byte a, byte b, byte c) {
    gen((byte)(a << 6 | b << 3 | c));
}

bool Compiler::isSectionDefined(SectionID id) const {
    return sections.find(id) != sections.end();
}

uint Compiler::sectionSize(SectionID id) const {
    return isSectionDefined(id) ? section(id).size() : 0;
}

ByteArray &Compiler::section(SectionID id) {
    if (!isSectionDefined(id))
        sections[id] = ByteArray();

    return sections.at(id);
}

const ByteArray &Compiler::section(SectionID id) const {
    return sections.at(id);
}

bool Compiler::isSymbolDefined(const std::string &name) const {
    return symbols.find(name) != symbols.end();
}

void Compiler::pushSymbol(const std::string &name, const std::string &baseSymbol, uint offset) {
    if (isSymbolDefined(name))
        throw std::runtime_error("symbol '" + name + "' is already defined");

    symbols[name] = Symbol{ baseSymbol, offset };
}

void Compiler::pushReloc(const Reloc &reloc) {
    relocs << reloc;
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
