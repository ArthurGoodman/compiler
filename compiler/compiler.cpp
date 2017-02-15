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
    instr(0x50 + reg);
}

void Compiler::push(const MemRef &ref) {
    instr(0xff, 6, ref);
}

void Compiler::push(byte value) {
    instr(0x6a, value);
}

void Compiler::push(int value) {
    instr(0x68, value);
}

void Compiler::push(const SymRef &ref) {
    instr(0x68, ref);
}

void Compiler::pop(Register reg) {
    instr(0x58 + reg);
}

void Compiler::pop(const MemRef &ref) {
    instr(0x8f, 0, ref);
}

void Compiler::call(int disp) {
    instr(0xe8, disp);
}

void Compiler::call(const SymRef &ref) {
    instr(0xe8, ref);
}

void Compiler::call(Register reg) {
    instr(0xff, 2, reg);
}

void Compiler::call(const MemRef &ref) {
    instr(0xff, 2, ref);
}

void Compiler::mov(Register src, Register dst) {
    instr(0x89, src, dst);
}

void Compiler::mov(int imm, Register dst) {
    instr(0xb8 + dst, imm);
}

void Compiler::mov(const SymRef &src, Register dst) {
    instr(0xb8 + dst, src);
}

void Compiler::mov(int imm, const MemRef &dst) {
    instr(0xc7, 0, dst, imm);
}

void Compiler::mov(const SymRef &ref, const MemRef &dst) {
    instr(0xc7, 0, dst, ref);
}

void Compiler::mov(Register src, const MemRef &dst) {
    instr(0x89, src, dst);
}

void Compiler::mov(const MemRef &src, Register dst) {
    instr(0x8b, dst, src);
}

void Compiler::lea(const MemRef &src, Register dst) {
    instr(0x8d, dst, src);
}

void Compiler::add(byte imm, Register dst) {
    instr(0x83, 0, dst, imm);
}

void Compiler::add(int imm, Register dst) {
    if (dst == EAX)
        instr(0x05, imm);
    else
        instr(0x81, 0, dst, imm);
}

void Compiler::add(byte imm, const MemRef &dst) {
    instr(0x83, 0, dst, imm);
}

void Compiler::addb(byte imm, const MemRef &dst) {
    instr(0x80, 0, dst, imm);
}

void Compiler::add(int imm, const MemRef &dst) {
    instr(0x81, 0, dst, imm);
}

void Compiler::add(Register src, const MemRef &dst) {
    instr(0x01, src, dst);
}

void Compiler::add(const MemRef &src, Register dst) {
    instr(0x03, dst, src);
}

void Compiler::sub(byte imm, Register dst) {
    instr(0x83, 5, dst, imm);
}

void Compiler::sub(int imm, Register dst) {
    if (dst == EAX)
        instr(0x2d, imm);
    else
        instr(0x81, 5, dst, imm);
}

void Compiler::sub(byte imm, const MemRef &dst) {
    instr(0x83, 5, dst, imm);
}

void Compiler::subb(byte imm, const MemRef &dst) {
    instr(0x80, 5, dst, imm);
}

void Compiler::sub(int imm, const MemRef &dst) {
    instr(0x81, 5, dst, imm);
}

void Compiler::sub(Register src, const MemRef &dst) {
    instr(0x29, src, dst);
}

void Compiler::sub(const MemRef &src, Register dst) {
    instr(0x2b, dst, src);
}

void Compiler::leave() {
    instr(0xc9);
}

void Compiler::ret() {
    instr(0xc3);
}

void Compiler::nop() {
    instr(0x90);
}

void Compiler::flds(const MemRef &ref) {
    instr(0xd9, 0, ref);
}

void Compiler::fldl(const MemRef &ref) {
    instr(0xdd, 0, ref);
}

void Compiler::fld(FPURegister reg) {
    instr(0xd9, (byte)(0xc0 + reg));
}

void Compiler::fsts(const MemRef &ref) {
    instr(0xd9, 2, ref);
}

void Compiler::fstl(const MemRef &ref) {
    instr(0xdd, 2, ref);
}

void Compiler::fst(FPURegister reg) {
    instr(0xdd, (byte)(0xd0 + reg));
}

void Compiler::fstps(const MemRef &ref) {
    instr(0xd9, 3, ref);
}

void Compiler::fstpl(const MemRef &ref) {
    instr(0xdd, 3, ref);
}

void Compiler::fstp(FPURegister reg) {
    instr(0xdd, (byte)(0xd8 + reg));
}

void Compiler::fadds(const MemRef &ref) {
    instr(0xd8, 0, ref);
}

void Compiler::faddl(const MemRef &ref) {
    instr(0xdc, 0, ref);
}

void Compiler::fadd(FPURegister src, FPURegister dst) {
    if (src == ST0)
        instr(0xdc, (byte)(0xc0 + dst));
    else if (dst == ST0)
        instr(0xd8, (byte)(0xc0 + src));
    else
        throw std::runtime_error("one of registers must be %st(0)");
}

void Compiler::faddp(FPURegister dst) {
    instr(0xde, (byte)(0xc0 + dst));
}

void Compiler::faddp() {
    instr(0xde, (byte)0xc1);
}

void Compiler::fiaddl(const MemRef &ref) {
    instr(0xda, 0, ref);
}

void Compiler::fsubs(const MemRef &ref) {
    instr(0xd8, 4, ref);
}

void Compiler::fsubl(const MemRef &ref) {
    instr(0xdc, 4, ref);
}

void Compiler::fsub(FPURegister src, FPURegister dst) {
    if (src == ST0)
        instr(0xdc, (byte)(0xe8 + dst));
    else if (dst == ST0)
        instr(0xd8, (byte)(0xe0 + src));
    else
        throw std::runtime_error("one of registers must be %st(0)");
}

void Compiler::fsubp(FPURegister dst) {
    instr(0xde, (byte)(0xe8 + dst));
}

void Compiler::fsubp() {
    instr(0xde, (byte)0xe9);
}

void Compiler::fisubl(const MemRef &ref) {
    instr(0xda, 4, ref);
}

void Compiler::fmuls(const MemRef &ref) {
    instr(0xd8, 1, ref);
}

void Compiler::fmull(const MemRef &ref) {
    instr(0xdc, 1, ref);
}

void Compiler::fmul(FPURegister src, FPURegister dst) {
    if (src == ST0)
        instr(0xdc, (byte)(0xc8 + dst));
    else if (dst == ST0)
        instr(0xd8, (byte)(0xc8 + src));
    else
        throw std::runtime_error("one of registers must be %st(0)");
}

void Compiler::fmulp(FPURegister dst) {
    instr(0xde, (byte)(0xc8 + dst));
}

void Compiler::fmulp() {
    instr(0xde, (byte)0xc9);
}

void Compiler::fimull(const MemRef &ref) {
    instr(0xda, 1, ref);
}

void Compiler::fdivs(const MemRef &ref) {
    instr(0xd8, 6, ref);
}

void Compiler::fdivl(const MemRef &ref) {
    instr(0xdc, 6, ref);
}

void Compiler::fdiv(FPURegister src, FPURegister dst) {
    if (src == ST0)
        instr(0xdc, (byte)(0xf8 + dst));
    else if (dst == ST0)
        instr(0xd8, (byte)(0xf0 + src));
    else
        throw std::runtime_error("one of registers must be %st(0)");
}

void Compiler::fdivp(FPURegister dst) {
    instr(0xde, (byte)(0xf8 + dst));
}

void Compiler::fdivp() {
    instr(0xde, (byte)0xf9);
}

void Compiler::fidivl(const MemRef &ref) {
    instr(0xda, 6, ref);
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

Function Compiler::compileFunction() {
    return std::move(Function(std::move(section(TEXT))));
}

void Compiler::instr(byte op) {
    gen(op);
}

void Compiler::instr(byte op, byte imm) {
    gen(op);
    gen(imm);
}

void Compiler::instr(byte op, int imm) {
    gen(op);
    gen(imm);
}

void Compiler::instr(byte op, const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    instr(op, ref.offset);

    pushReloc(Reloc{ ref.name, ref.type, sectionSize(TEXT) - 4 });
}

void Compiler::instr(byte op, byte reg, Register rm) {
    instr(op, reg, MemRef{ Reg, rm, 0, 0, 0, 0 });
}

void Compiler::instr(byte op, byte reg, Register rm, byte imm) {
    instr(op, reg, rm);
    gen(imm);
}

void Compiler::instr(byte op, byte reg, Register rm, int imm) {
    instr(op, reg, rm);
    gen(imm);
}

void Compiler::instr(byte op, byte reg, Register rm, const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    instr(op, reg, rm, ref.offset);

    pushReloc(Reloc{ ref.name, ref.type, sectionSize(TEXT) - 4 });
}

void Compiler::instr(byte op, byte reg, const MemRef &rm) {
    gen(op);

    gen(composeByte(rm.mod, reg, rm.rm));

    if (rm.scale != 0)
        gen(composeByte(log2(rm.scale), rm.index, rm.base));

    if (rm.mod == Disp8)
        gen((byte)rm.disp);
    else if (rm.mod == Disp32 || (rm.mod == Disp0 && (rm.rm == 5 || (rm.rm == 4 && rm.base == 5))))
        gen(rm.disp);
}

void Compiler::instr(byte op, byte reg, const MemRef &rm, byte imm) {
    instr(op, reg, rm);
    gen(imm);
}

void Compiler::instr(byte op, byte reg, const MemRef &rm, int imm) {
    instr(op, reg, rm);
    gen(imm);
}

void Compiler::instr(byte op, byte reg, const MemRef &rm, const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    instr(op, reg, rm, ref.offset);

    pushReloc(Reloc{ ref.name, ref.type, sectionSize(TEXT) - 4 });
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
