#pragma once

#include <cstring>
#include <map>
#include <string>

#include "x86_64_compiler/bytearray.hpp"

namespace x86_64 {

enum X86Register
{
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI,
};

enum X64Register
{
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
};

class Compiler final
{
private: // types
    struct Symbol
    {
        std::string base_symbol;
        std::size_t offset;
    };

    enum SectionID
    {
        TEXT = 1,
        DATA,
        BSS,
        RDATA,
        EDATA,
        IDATA,
        RELOC,
    };

public: // methods
    void rdata(const std::string &name, const uint8_t *data, std::size_t size);

    template <class T>
    void rdata(const std::string &name, T value);

    void data(const std::string &name, const uint8_t *data, std::size_t size);

    template <class T>
    void data(const std::string &name, T value);

    void bss(const std::string &name, std::size_t size);

    const ByteArray &getCode() const;

    void constant(int8_t value);
    void constant(int32_t value);
    void constant(double value);

private: // methods
    template <class T>
    void gen(T value);

    const ByteArray &section(SectionID id) const;
    ByteArray &section(SectionID id);

    bool isSectionDefined(SectionID id) const;
    std::size_t sectionSize(SectionID id) const;

    bool isSymbolDefined(const std::string &name) const;

    void pushSymbol(
        const std::string &name,
        const std::string &base_symbol,
        std::size_t offset);

    template <class T>
    static bool isByte(T value);

private: // fields
    std::map<SectionID, ByteArray> sections;
    std::map<std::string, Symbol> symbols;
};

template <class T>
void Compiler::rdata(const std::string &name, T value)
{
    rdata(name, reinterpret_cast<const uint8_t *>(&value), sizeof(value));
}

template <>
inline void Compiler::rdata(const std::string &name, const char *value)
{
    rdata(name, reinterpret_cast<const uint8_t *>(&value), strlen(value));
}

template <class T>
void Compiler::data(const std::string &name, T value)
{
    data(name, reinterpret_cast<const uint8_t *>(&value), sizeof(value));
}

template <>
inline void Compiler::data(const std::string &name, const char *value)
{
    data(name, reinterpret_cast<const uint8_t *>(&value), strlen(value));
}

template <class T>
void Compiler::gen(T value)
{
    section(TEXT).push(value);
}

template <class T>
bool Compiler::isByte(T value)
{
    return value >= -128 && value <= 127;
}

} // namespace x86_64
