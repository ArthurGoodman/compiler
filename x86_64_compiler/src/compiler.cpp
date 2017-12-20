#include "x86_64_compiler/compiler.hpp"

namespace x86_64 {

void Compiler::rdata(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    std::size_t offset = section(RDATA).size();
    section(RDATA).push(data, size);
    pushSymbol(name, ".rdata", offset);
}

void Compiler::data(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    std::size_t offset = section(DATA).size();
    section(DATA).push(data, size);
    pushSymbol(name, ".data", offset);
}

void Compiler::bss(const std::string &name, std::size_t size)
{
    std::size_t offset = section(BSS).size();
    section(BSS).push(nullptr, size);
    pushSymbol(name, ".bss", offset);
}

const ByteArray &Compiler::getCode() const
{
    return section(TEXT);
}

Compiler::Ref Compiler::reg(X86Register reg) const
{
    return Ref(markReg(reg));
}

Compiler::Ref Compiler::reg(X64Register reg) const
{
    return Ref(markReg(reg));
}

Compiler::Ref Compiler::mem(X86Register reg) const
{
    return Ref(0, NOREG, markReg(reg));
}

Compiler::Ref Compiler::mem(X64Register reg) const
{
    return Ref(0, NOREG, markReg(reg));
}

Compiler::Ref Compiler::mem(X86Register index, int8_t scale) const
{
    return Ref(scale, markReg(index), NOREG);
}

Compiler::Ref Compiler::mem(X64Register index, int8_t scale) const
{
    return Ref(scale, markReg(index), NOREG);
}

Compiler::Ref Compiler::mem(X86Register base, X86Register index, int8_t scale)
    const
{
    return Ref(scale, markReg(index), markReg(base));
}

Compiler::Ref Compiler::mem(X64Register base, X64Register index, int8_t scale)
    const
{
    return Ref(scale, markReg(index), markReg(base));
}

Compiler::Ref Compiler::abs(const std::string &name)
{
    return Ref(Ref::SymRef::Type::Abs, name);
}

Compiler::Ref Compiler::rel(const std::string &name)
{
    return Ref(Ref::SymRef::Type::Rel, name);
}

void Compiler::constant(int8_t value)
{
    gen(value);
}

void Compiler::constant(int32_t value)
{
    gen(value);
}

void Compiler::constant(double value)
{
    gen(value);
}

const ByteArray &Compiler::section(SectionID id) const
{
    return sections.at(id);
}

ByteArray &Compiler::section(SectionID id)
{
    return sections[id];
}

bool Compiler::isSectionDefined(SectionID id) const
{
    return sections.find(id) != sections.end();
}

std::size_t Compiler::sectionSize(SectionID id) const
{
    return isSectionDefined(id) ? section(id).size() : 0;
}

bool Compiler::isSymbolDefined(const std::string &name) const
{
    return symbols.find(name) != symbols.end();
}

void Compiler::pushSymbol(
    const std::string &name,
    const std::string &base_symbol,
    std::size_t offset)
{
    if (isSymbolDefined(name))
        throw std::runtime_error("symbol '" + name + "' is already defined");

    symbols[name] = Symbol{base_symbol, offset};
}

} // namespace x86_64
