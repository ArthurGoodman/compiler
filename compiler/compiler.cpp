#include "compiler.h"

#include "common.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <exception>

namespace x86 {

Compiler::SymRef Compiler::SymRef::operator+(int offset) const {
    return SymRef{name, type, this->offset + offset};
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
    return SymRef{name, RefAbs, 0};
}

Compiler::SymRef Compiler::rel(const std::string &name) const {
    return SymRef{name, RefRel, 0};
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

void Compiler::push(const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    gen((byte)0x68);

    uint offset = sectionSize(TEXT);

    gen(ref.offset);

    pushReloc(Reloc{ref.name, ref.type, offset});
}

void Compiler::push(Register reg) {
    push(MemRef(reg));
}

void Compiler::call(const MemRef &ref) {
    regRMInstruction(0xff, ref, EDX);
}

void Compiler::call(int value) {
    gen((byte)0xe8);
    gen(value);
}

void Compiler::call(const SymRef &ref) {
    if (!isSymbolDefined(ref.name))
        throw std::runtime_error("undefined symbol '" + ref.name + "'");

    gen((byte)0xe8);

    uint offset = sectionSize(TEXT);

    gen(ref.offset);

    pushReloc(Reloc{ref.name, ref.type, offset});
}

void Compiler::call(Register reg) {
    call(MemRef(reg));
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

void Compiler::regRMInstruction(byte op, const MemRef &op1, const MemRef &op2) {
    assert(!(op1.isAddress() && op2.isAddress()) && "too many memory references");

    gen((byte)(op + (op2.isAddress() ? 0x2 : 0x0)));

    if (!op1.isAddress() && !op2.isAddress()) {
        composeByte(Reg, op2, op1);
        return;
    }

    const MemRef &r = op1.isAddress() ? op2 : op1;
    const MemRef &rm = op1.isAddress() ? op1 : op2;

    switch (rm.getDispSize()) {
    case 0:
        if (rm.getScale() != 0) {
            composeByte(Disp0, r, ESP);
            composeByte(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            composeByte(Disp0, r, rm);
        break;

    case 1:
        if (rm.getScale() != 0) {
            composeByte(Disp8, r, ESP);
            composeByte(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            composeByte(Disp8, r, rm);
        gen((byte)rm.getDisp());
        break;

    case 4:
        if (rm.getBase() == NOREG)
            composeByte(Disp0, r, EBP);
        else if (rm.getScale() != 0) {
            composeByte(Disp32, r, ESP);
            composeByte(log2(rm.getScale()), rm.getIndex(), rm.getBase());
        } else
            composeByte(Disp32, r, rm);
        gen(rm.getDisp());
        break;

    default:
        break;
    }
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

    symbols[name] = Symbol{baseSymbol, offset};
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
