#pragma once

#include <cstring>
#include <map>
#include <string>

#include "x86_64_compiler/bytearray.hpp"

namespace x86_64 {

constexpr int16_t c_reg_mask = 0xf;
constexpr int16_t c_x64_reg_flag = 0x10;

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
    NOREG = -1,

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
    struct Ref
    {
    public: // types
        enum class Type
        {
            Reg,
            Mem,
            Sym,
        };

        struct MemRef
        {
            int8_t scale;
            int16_t index;
            int16_t base;
            int32_t disp;
        };

        struct SymRef
        {
            enum class Type
            {
                Abs,
                Rel,
            };

            Type type;
            const char *name;
            int32_t offset;
        };

    public: // methods
        Ref(int16_t reg)
            : type{Type::Reg}
            , reg{reg}
        {
        }

        explicit Ref(int8_t scale, int16_t index, int16_t base)
            : type{Type::Mem}
            , mem{scale, index, base, 0}
        {
        }

        explicit Ref(SymRef::Type type, const std::string &name)
            : type{Type::Sym}
            , sym{type, strdup(name.c_str()), 0}
        {
        }

        Ref(const Ref &mem) = default;

        ~Ref()
        {
            if (type != Type::Sym)
            {
                delete sym.name;
            }
        }

        Ref operator+(int32_t offset) const
        {
            Ref ref = *this;

            switch (type)
            {
            case Type::Reg:
                ref.type = Type::Mem;
                ref.mem = MemRef{0, NOREG, ref.reg, offset};
                break;

            case Type::Mem:
                ref.mem.disp += offset;
                break;

            case Type::Sym:
                ref.sym.offset += offset;
                break;
            }

            return ref;
        }

        friend Ref operator+(int32_t offset, const Ref &ref)
        {
            return ref + offset;
        }

    public: // fields
        Type type;

        union
        {
            int16_t reg;
            MemRef mem;
            SymRef sym;
        };
    };

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

    Ref reg(X86Register reg) const;
    Ref reg(X64Register reg) const;

    Ref mem(X86Register reg) const;
    Ref mem(X64Register reg) const;
    Ref mem(X86Register index, int8_t scale) const;
    Ref mem(X64Register index, int8_t scale) const;
    Ref mem(X86Register base, X86Register index, int8_t scale) const;
    Ref mem(X64Register base, X64Register index, int8_t scale) const;

    Ref abs(const std::string &name);
    Ref rel(const std::string &name);

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

    static int16_t markReg(X86Register reg);
    static int16_t markReg(X64Register reg);

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
inline bool Compiler::isByte(T value)
{
    return value >= -128 && value <= 127;
}

inline int16_t Compiler::markReg(X86Register reg)
{
    return static_cast<int16_t>(reg) & ~c_x64_reg_flag;
}

inline int16_t Compiler::markReg(X64Register reg)
{
    return static_cast<int16_t>(reg) | c_x64_reg_flag;
}

} // namespace x86_64
