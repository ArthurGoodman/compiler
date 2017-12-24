#include "x86_64_compiler/compiler.hpp"

#include <stdexcept>

namespace x86_64 {

void Compiler::reset()
{
    m_sections.clear();
    m_symbols.clear();
}

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

Compiler::RegRef Compiler::reg(const RegRef &reg) const
{
    return reg;
}

Compiler::MemRef Compiler::mem(int32_t disp) const
{
    return disp + MemRef(0, NOREG, NOREG);
}

Compiler::MemRef Compiler::mem(const RegRef &reg) const
{
    return MemRef(0, NOREG, reg);
}

Compiler::MemRef Compiler::mem(const RegRef &index, int8_t scale) const
{
    return MemRef(scale, index, NOREG);
}

Compiler::MemRef Compiler::mem(
    const RegRef &base,
    const RegRef &index,
    int8_t scale) const
{
    return MemRef(scale, index, base);
}

Compiler::SymRef Compiler::abs(const std::string &name)
{
    return SymRef(SymRef::Type::Abs, name);
}

Compiler::SymRef Compiler::rel(const std::string &name)
{
    return SymRef(SymRef::Type::Rel, name);
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

void Compiler::mov(const Ref &src, const Ref &dst)
{
    gen(0x0);
}

void Compiler::movb(uint8_t imm, const Ref &dst)
{
    gen(0x1);
}

void Compiler::movl(uint32_t imm, const Ref &dst)
{
    gen(0x2);
}

void Compiler::movq(uint64_t imm, const Ref &dst)
{
    gen(0x3);
}

const ByteArray &Compiler::section(SectionID id) const
{
    if (!isSectionDefined(id))
    {
        static const char *section_names[] = {
            "", "TEXT", "DATA", "BSS", "RDATA", "EDATA", "IDATA", "RELOC",
        };

        throw std::runtime_error(
            "section " + std::string(section_names[id]) + " is not defined");
    }

    return m_sections.at(id);
}

ByteArray &Compiler::section(SectionID id)
{
    return m_sections[id];
}

bool Compiler::isSectionDefined(SectionID id) const
{
    return m_sections.find(id) != m_sections.end();
}

std::size_t Compiler::sectionSize(SectionID id) const
{
    return isSectionDefined(id) ? section(id).size() : 0;
}

bool Compiler::isSymbolDefined(const std::string &name) const
{
    return m_symbols.find(name) != m_symbols.end();
}

void Compiler::pushSymbol(
    const std::string &name,
    const std::string &base_symbol,
    std::size_t offset)
{
    if (isSymbolDefined(name))
        throw std::runtime_error("symbol '" + name + "' is already defined");

    m_symbols[name] = Symbol{base_symbol, offset};
}

} // namespace x86_64
