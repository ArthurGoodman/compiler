#pragma once

#include <cstring>
#include <map>
#include <string>

#include "x86_64_compiler/bytearray.hpp"

namespace x86_64 {

constexpr int16_t c_reg_mask = 0xf;
constexpr int16_t c_x64_reg_flag = 0x10;

namespace detail {

constexpr int16_t NOREG = -1;

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

struct RegRef
{
public: // methods
    constexpr RegRef()
        : reg{NOREG}
    {
    }

    constexpr RegRef(X86Register reg)
        : reg{markReg(reg)}
    {
    }

    constexpr RegRef(X64Register reg)
        : reg{markReg(reg)}
    {
    }

private: // methods
    static constexpr int16_t markReg(X86Register reg)
    {
        return static_cast<int16_t>(reg) & ~c_x64_reg_flag;
    }

    static constexpr int16_t markReg(X64Register reg)
    {
        return static_cast<int16_t>(reg) | c_x64_reg_flag;
    }

public: // fields
    int16_t reg;
};

} // namespace detail

constexpr detail::RegRef NOREG;

constexpr detail::RegRef EAX(detail::EAX);
constexpr detail::RegRef ECX(detail::ECX);
constexpr detail::RegRef EDX(detail::EDX);
constexpr detail::RegRef EBX(detail::EBX);
constexpr detail::RegRef ESP(detail::ESP);
constexpr detail::RegRef EBP(detail::EBP);
constexpr detail::RegRef ESI(detail::ESI);
constexpr detail::RegRef EDI(detail::EDI);

constexpr detail::RegRef RAX(detail::RAX);
constexpr detail::RegRef RCX(detail::RCX);
constexpr detail::RegRef RDX(detail::RDX);
constexpr detail::RegRef RBX(detail::RBX);
constexpr detail::RegRef RSP(detail::RSP);
constexpr detail::RegRef RBP(detail::RBP);
constexpr detail::RegRef RSI(detail::RSI);
constexpr detail::RegRef RDI(detail::RDI);
constexpr detail::RegRef R8(detail::R8);
constexpr detail::RegRef R9(detail::R9);
constexpr detail::RegRef R10(detail::R10);
constexpr detail::RegRef R11(detail::R11);
constexpr detail::RegRef R12(detail::R12);
constexpr detail::RegRef R13(detail::R13);
constexpr detail::RegRef R14(detail::R14);
constexpr detail::RegRef R15(detail::R15);

class Compiler final
{
private: // types
    using RegRef = detail::RegRef;

    struct MemRef
    {
    public: // methods
        MemRef(int8_t scale, const RegRef &index, const RegRef &base)
            : scale{scale}
            , index{index.reg}
            , base{base.reg}
            , disp{0}
        {
        }

        MemRef operator+(int32_t offset) const
        {
            return MemRef(*this, disp + offset);
        }

        MemRef operator-(int32_t offset) const
        {
            return MemRef(*this, disp - offset);
        }

        friend MemRef operator+(int32_t offset, const MemRef &ref)
        {
            return ref + offset;
        }

    private: // methods
        MemRef(const MemRef &ref, int32_t disp)
            : scale{ref.scale}
            , index{ref.index}
            , base{ref.base}
            , disp{disp}
        {
        }

    public: // fields
        int8_t scale;
        int16_t index;
        int16_t base;
        int32_t disp;
    };

    struct SymRef
    {
    public: // types
        enum class Type
        {
            Abs,
            Rel,
        };

    public: // methods
        SymRef(Type type, const std::string &name)
            : type{type}
            , name{strdup(name.c_str())}
            , offset{0}
        {
        }

        SymRef(const SymRef &ref)
            : name{nullptr}
        {
            *this = ref;
        }

        SymRef(SymRef &&ref)
            : name{nullptr}
        {
            *this = std::move(ref);
        }

        SymRef &operator=(const SymRef &ref)
        {
            delete name;

            type = ref.type;
            name = strdup(ref.name);
            offset = ref.offset;

            return *this;
        }

        SymRef &operator=(SymRef &&ref)
        {
            delete name;

            type = ref.type;
            name = ref.name;
            offset = ref.offset;

            ref.name = nullptr;

            return *this;
        }

        ~SymRef()
        {
            delete name;
        }

        SymRef operator+(int32_t offset) const
        {
            return SymRef(*this, this->offset + offset);
        }

        SymRef operator-(int32_t offset) const
        {
            return SymRef(*this, this->offset - offset);
        }

        friend SymRef operator+(int32_t offset, const SymRef &ref)
        {
            return ref + offset;
        }

    private: // methods
        SymRef(const SymRef &ref, int32_t offset)
            : type{ref.type}
            , name{strdup(ref.name)}
            , offset{offset}
        {
        }

    public: // fields
        Type type;
        const char *name;
        int32_t offset;
    };

    struct Ref
    {
    public: // types
        enum class Type
        {
            Reg,
            Mem,
            Sym,
        };

    public: // methods
        Ref(const RegRef &ref)
            : type{Type::Reg}
            , reg{ref.reg}
        {
        }

        Ref(const MemRef &ref)
            : type{Type::Mem}
            , mem{ref}
        {
        }

        Ref(const SymRef &ref)
            : type{Type::Sym}
            , sym{ref}
        {
        }

        Ref(const Ref &ref)
        {
            *this = ref;
        }

        Ref(Ref &&ref)
        {
            *this = std::move(ref);
        }

        Ref &operator=(const Ref &ref)
        {
            type = ref.type;

            switch (type)
            {
            case Type::Reg:
                reg = ref.reg;
                break;

            case Type::Mem:
                mem = ref.mem;
                break;

            case Type::Sym:
                sym = ref.sym;
                break;
            }

            return *this;
        }

        Ref &operator=(Ref &&ref)
        {
            type = ref.type;

            switch (type)
            {
            case Type::Reg:
                reg = std::move(ref.reg);
                break;

            case Type::Mem:
                mem = std::move(ref.mem);
                break;

            case Type::Sym:
                sym = std::move(ref.sym);
                break;
            }

            return *this;
        }

        ~Ref()
        {
            if (type == Type::Sym)
            {
                sym.~SymRef();
            }
        }

        Ref operator+(int32_t offset) const
        {
            Ref ref = *this;

            switch (ref.type)
            {
            case Type::Reg:
            case Type::Mem:
                ref.type = Type::Mem;
                ref.mem = ref.mem + offset;
                break;

            case Type::Sym:
                ref.sym = ref.sym + offset;
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
    void reset();

    void rdata(const std::string &name, const uint8_t *data, std::size_t size);

    template <class T>
    void rdata(const std::string &name, T value);

    void data(const std::string &name, const uint8_t *data, std::size_t size);

    template <class T>
    void data(const std::string &name, T value);

    void bss(const std::string &name, std::size_t size);

    const ByteArray &getCode() const;

    RegRef reg(const RegRef &reg) const;

    MemRef mem(int32_t disp) const;
    MemRef mem(const RegRef &reg) const;
    MemRef mem(const RegRef &index, int8_t scale) const;
    MemRef mem(const RegRef &base, const RegRef &index, int8_t scale) const;

    SymRef abs(const std::string &name);
    SymRef rel(const std::string &name);

    void constant(int8_t value);
    void constant(int32_t value);
    void constant(double value);

    void mov(const Ref &src, const Ref &dst);
    void movb(uint8_t imm, const Ref &dst);
    void movl(uint32_t imm, const Ref &dst);
    void movq(uint64_t imm, const Ref &dst);

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
    std::map<SectionID, ByteArray> m_sections;
    std::map<std::string, Symbol> m_symbols;
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

} // namespace x86_64
