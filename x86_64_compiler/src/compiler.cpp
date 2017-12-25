#include "x86_64_compiler/compiler.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <map>

namespace x86_64 {

constexpr uint8_t c_opcode_field_w = 1 << 0;
constexpr uint8_t c_opcode_field_d = 1 << 1;

constexpr uint8_t c_rex = 0x40;

constexpr uint8_t c_rex_field_b = 1 << 0;
constexpr uint8_t c_rex_field_x = 1 << 1;
constexpr uint8_t c_rex_field_r = 1 << 2;
constexpr uint8_t c_rex_field_w = 1 << 3;

class Compiler::Impl final
{
private: // types
    using RegRef = detail::RegRef;
    using Size = detail::Size;

    using MemRef = Compiler::MemRef;
    using SymRef = Compiler::SymRef;
    using Ref = Compiler::Ref;

    struct Imm
    {
    public: // methods
        Imm(uint8_t value);
        Imm(uint16_t value);
        Imm(uint32_t value);
        Imm(uint64_t value);

    public: // fields
        Size size;

        union
        {
            uint8_t byte;
            uint16_t word;
            uint32_t dword;
            uint64_t qword;
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
    void data(const std::string &name, const uint8_t *data, std::size_t size);
    void bss(const std::string &name, std::size_t size);

    const ByteArray &getCode() const;

    RegRef reg(const RegRef &reg) const;

    MemRef mem(int32_t disp) const;
    MemRef mem(const RegRef &reg) const;
    MemRef mem(const RegRef &index, int8_t scale) const;
    MemRef mem(const RegRef &base, const RegRef &index, int8_t scale) const;

    SymRef abs(const std::string &name);
    SymRef rel(const std::string &name);

    void constant(uint8_t value);
    void constant(uint16_t value);
    void constant(uint32_t value);
    void constant(double value);

    void mov(const Ref &src, const Ref &dst);
    void movb(uint8_t imm, const Ref &dst);
    void movw(uint16_t imm, const Ref &dst);
    void movl(uint32_t imm, const Ref &dst);
    void movq(uint64_t imm, const Ref &dst);

private: // methods
    void mov(const Imm &imm, const Ref &dst);

    void instr(uint8_t opcode, const Ref &op1, const Ref &op2);
    void instr(uint8_t opcode, const Imm &imm, const RegRef &reg_reg);
    void instr(uint8_t opcode, uint8_t ext, const Imm &imm, const Ref &op1);

    void genREXPreffix(
        const RegRef &reg,
        const RegRef &index,
        const RegRef &base);

    void genMemRef(const RegRef &reg_ref, const MemRef &mem_ref);
    void genSIBByte(const MemRef &mem_ref);

    void composeByte(int8_t a, int8_t b, int8_t c);

    template <class T>
    void gen(const T &value);

    const ByteArray &section(SectionID id) const;
    ByteArray &section(SectionID id);

    bool isSectionDefined(SectionID id) const;
    std::size_t sectionSize(SectionID id) const;

    bool isSymbolDefined(const std::string &name) const;

    void pushSymbol(
        const std::string &name,
        const std::string &base_symbol,
        std::size_t offset);

public:
    template <class T>
    static bool isByte(T value);

    template <class T>
    static bool isDword(T value);

private: // fields
    std::map<SectionID, ByteArray> m_sections;
    std::map<std::string, Symbol> m_symbols;
};

template <class T>
void Compiler::Impl::gen(const T &value)
{
    section(TEXT).push(value);
}

template <>
inline void Compiler::Impl::gen(const Imm &imm)
{
    switch (imm.size)
    {
    case Size::Byte:
        gen(imm.byte);
        break;

    case Size::Word:
        gen(imm.word);
        break;

    case Size::Dword:
        gen(imm.dword);
        break;

    case Size::Qword:
        gen(imm.qword);
        break;

    default:
        break;
    }
}

template <class T>
bool Compiler::Impl::isByte(T value)
{
    return value >= -128 && value <= 127;
}

template <class T>
bool Compiler::Impl::isDword(T value)
{
    return static_cast<int64_t>(value) >= INT32_MIN &&
           static_cast<int64_t>(value) <= INT32_MAX;
}

bool detail::RegRef::operator==(const detail::RegRef &ref) const
{
    return size == ref.size && reg == ref.reg;
}

Compiler::MemRef::MemRef(int8_t scale, const RegRef &index, const RegRef &base)
    : scale{scale}
    , index{index}
    , base{base}
    , disp{0}
{
}

Compiler::MemRef Compiler::MemRef::operator-(int32_t offset) const
{
    return MemRef(*this, disp - offset);
}

Compiler::MemRef::MemRef(const MemRef &ref, int32_t disp)
    : scale{ref.scale}
    , index{ref.index}
    , base{ref.base}
    , disp{disp}
{
}

Compiler::MemRef Compiler::MemRef::operator+(int32_t offset) const
{
    return MemRef(*this, disp + offset);
}

Compiler::MemRef operator+(int32_t offset, const Compiler::MemRef &ref)
{
    return ref + offset;
}

Compiler::SymRef::SymRef(Type type, const std::string &name)
    : type{type}
    , name{strdup(name.c_str())}
    , offset{0}
{
}

Compiler::SymRef::SymRef(const SymRef &ref)
    : name{nullptr}
{
    *this = ref;
}

Compiler::SymRef::SymRef(SymRef &&ref)
    : name{nullptr}
{
    *this = std::move(ref);
}

Compiler::SymRef &Compiler::SymRef::operator=(const SymRef &ref)
{
    delete name;

    type = ref.type;
    name = strdup(ref.name);
    offset = ref.offset;

    return *this;
}

Compiler::SymRef &Compiler::SymRef::operator=(SymRef &&ref)
{
    delete name;

    type = ref.type;
    name = ref.name;
    offset = ref.offset;

    ref.name = nullptr;

    return *this;
}

Compiler::SymRef::~SymRef()
{
    delete name;
}

Compiler::SymRef Compiler::SymRef::operator+(int32_t offset) const
{
    return SymRef(*this, this->offset + offset);
}

Compiler::SymRef Compiler::SymRef::operator-(int32_t offset) const
{
    return SymRef(*this, this->offset - offset);
}

Compiler::SymRef::SymRef(const SymRef &ref, int32_t offset)
    : type{ref.type}
    , name{strdup(ref.name)}
    , offset{offset}
{
}

Compiler::SymRef operator+(int32_t offset, const Compiler::SymRef &ref)
{
    return ref + offset;
}

Compiler::Ref::Ref(const RegRef &ref)
    : type{Type::Reg}
    , reg{ref}
{
}

Compiler::Ref::Ref(const MemRef &ref)
    : type{Type::Mem}
    , mem{ref}
{
}

Compiler::Ref::Ref(const SymRef &ref)
    : type{Type::Sym}
    , sym{ref}
{
}

Compiler::Ref::Ref(const Ref &ref)
{
    *this = ref;
}

Compiler::Ref::Ref(Ref &&ref)
{
    *this = std::move(ref);
}

Compiler::Ref &Compiler::Ref::operator=(const Ref &ref)
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

Compiler::Ref &Compiler::Ref::operator=(Ref &&ref)
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

Compiler::Ref::~Ref()
{
    if (type == Type::Sym)
    {
        sym.~SymRef();
    }
}

Compiler::Ref Compiler::Ref::operator+(int32_t offset) const
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

Compiler::Ref operator+(int32_t offset, const Compiler::Ref &ref)
{
    return ref + offset;
}

Compiler::Compiler()
    : m_impl{new Compiler::Impl}
{
}

Compiler::~Compiler()
{
}

void Compiler::reset()
{
    m_impl->reset();
}

void Compiler::rdata(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    m_impl->rdata(name, data, size);
}

void Compiler::data(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    m_impl->data(name, data, size);
}

void Compiler::bss(const std::string &name, std::size_t size)
{
    m_impl->bss(name, size);
}

const ByteArray &Compiler::getCode() const
{
    return m_impl->getCode();
}

Compiler::RegRef Compiler::reg(const RegRef &reg) const
{
    return m_impl->reg(reg);
}

Compiler::MemRef Compiler::mem(int32_t disp) const
{
    return m_impl->mem(disp);
}

Compiler::MemRef Compiler::mem(const RegRef &reg) const
{
    return m_impl->mem(reg);
}

Compiler::MemRef Compiler::mem(const RegRef &index, int8_t scale) const
{
    return m_impl->mem(index, scale);
}

Compiler::MemRef Compiler::mem(
    const RegRef &base,
    const RegRef &index,
    int8_t scale) const
{
    return m_impl->mem(base, index, scale);
}

Compiler::SymRef Compiler::abs(const std::string &name)
{
    return m_impl->abs(name);
}

Compiler::SymRef Compiler::rel(const std::string &name)
{
    return m_impl->rel(name);
}

void Compiler::constant(uint8_t value)
{
    return m_impl->constant(value);
}

void Compiler::constant(uint16_t value)
{
    return m_impl->constant(value);
}

void Compiler::constant(uint32_t value)
{
    return m_impl->constant(value);
}

void Compiler::constant(double value)
{
    return m_impl->constant(value);
}

void Compiler::mov(const Ref &src, const Ref &dst)
{
    return m_impl->mov(src, dst);
}

void Compiler::movb(uint8_t imm, const Ref &dst)
{
    return m_impl->movb(imm, dst);
}

void Compiler::movw(uint16_t imm, const Ref &dst)
{
    return m_impl->movw(imm, dst);
}

void Compiler::movl(uint32_t imm, const Ref &dst)
{
    return m_impl->movl(imm, dst);
}

void Compiler::movq(uint64_t imm, const Ref &dst)
{
    return m_impl->movq(imm, dst);
}

Compiler::Impl::Imm::Imm(uint8_t value)
    : size{Size::Byte}
    , byte{value}
{
}

Compiler::Impl::Imm::Imm(uint16_t value)
    : size{Size::Word}
    , word{value}
{
}

Compiler::Impl::Imm::Imm(uint32_t value)
    : size{Size::Dword}
    , dword{value}
{
}

Compiler::Impl::Imm::Imm(uint64_t value)
    : size{Size::Qword}
    , qword{value}
{
}

void Compiler::Impl::reset()
{
    m_sections.clear();
    m_symbols.clear();
}

void Compiler::Impl::rdata(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    std::size_t offset = section(RDATA).size();
    section(RDATA).push(data, size);
    pushSymbol(name, ".rdata", offset);
}

void Compiler::Impl::data(
    const std::string &name,
    const uint8_t *data,
    std::size_t size)
{
    std::size_t offset = section(DATA).size();
    section(DATA).push(data, size);
    pushSymbol(name, ".data", offset);
}

void Compiler::Impl::bss(const std::string &name, std::size_t size)
{
    std::size_t offset = section(BSS).size();
    section(BSS).push(nullptr, size);
    pushSymbol(name, ".bss", offset);
}

const ByteArray &Compiler::Impl::getCode() const
{
    return section(TEXT);
}

Compiler::RegRef Compiler::Impl::reg(const RegRef &reg) const
{
    return reg;
}

Compiler::MemRef Compiler::Impl::mem(int32_t disp) const
{
    return disp + MemRef(0, NOREG, NOREG);
}

Compiler::MemRef Compiler::Impl::mem(const RegRef &reg) const
{
    return MemRef(0, NOREG, reg);
}

Compiler::MemRef Compiler::Impl::mem(const RegRef &index, int8_t scale) const
{
    return MemRef(scale, index, NOREG);
}

Compiler::MemRef Compiler::Impl::mem(
    const RegRef &base,
    const RegRef &index,
    int8_t scale) const
{
    return MemRef(scale, index, base);
}

Compiler::SymRef Compiler::Impl::abs(const std::string &name)
{
    return SymRef(SymRef::Type::Abs, name);
}

Compiler::SymRef Compiler::Impl::rel(const std::string &name)
{
    return SymRef(SymRef::Type::Rel, name);
}

void Compiler::Impl::constant(uint8_t value)
{
    gen(value);
}

void Compiler::Impl::constant(uint16_t value)
{
    gen(value);
}

void Compiler::Impl::constant(uint32_t value)
{
    gen(value);
}

void Compiler::Impl::constant(double value)
{
    gen(value);
}

void Compiler::Impl::mov(const Ref &src, const Ref &dst)
{
    if (src.type == Ref::Type::Reg)
    {
        instr(0x88, src, dst);
    }
    else
    {
        if ((src.type == Ref::Type::Reg && src.reg.reg == 0 &&
             dst.type == Ref::Type::Mem && dst.mem.base == NOREG &&
             dst.mem.index == NOREG) ||
            (dst.type == Ref::Type::Reg && dst.reg.reg == 0 &&
             src.type == Ref::Type::Mem && src.mem.base == NOREG &&
             src.mem.index == NOREG))
        {
            instr(0xa0, src, dst);
        }
        else
        {
            instr(0x89, src, dst);
        }
    }
}

void Compiler::Impl::movb(uint8_t imm, const Ref &dst)
{
    mov(imm, dst);
}

void Compiler::Impl::movw(uint16_t imm, const Ref &dst)
{
    mov(imm, dst);
}

void Compiler::Impl::movl(uint32_t imm, const Ref &dst)
{
    mov(imm, dst);
}

void Compiler::Impl::movq(uint64_t imm, const Ref &dst)
{
    mov(imm, dst);
}

void Compiler::Impl::mov(const Imm &imm, const Ref &dst)
{
    if (dst.type == Ref::Type::Reg)
    {
        if (dst.reg.size == Size::Byte)
        {
            instr(0xb0, imm, dst.reg);
        }
        else
        {
            if (dst.reg.size == Size::Qword && isDword(imm.qword))
            {
                instr(0xc6, 0, Imm(static_cast<uint32_t>(imm.dword)), dst.reg);
            }
            else
            {
                instr(0xb8, imm, dst.reg);
            }
        }
    }
    else
    {
        instr(0xc6, 0, imm, dst);
    }
}

void Compiler::Impl::instr(uint8_t opcode, const Ref &op1, const Ref &op2)
{
    RegRef reg_ref = op1.type == Ref::Type::Reg ? op1.reg : op2.reg;
    Ref rm_ref = op1.type == Ref::Type::Reg ? op2 : op1;

    if (op2.type == Ref::Type::Reg && op1.type != Ref::Type::Reg)
    {
        opcode += c_opcode_field_d;
    }

    if (reg_ref.size != Size::Byte)
    {
        opcode += c_opcode_field_w;
    }

    genREXPreffix(
        reg_ref,
        (rm_ref.type == Ref::Type::Mem && rm_ref.mem.scale != 0)
            ? rm_ref.mem.index
            : NOREG,
        rm_ref.type == Ref::Type::Reg ? rm_ref.reg : NOREG);

    gen(opcode);

    if (rm_ref.type == Ref::Type::Reg)
    {
        composeByte(3, reg_ref.reg & ~R8.reg, rm_ref.reg.reg & ~R8.reg);
    }
    else
    {
        genMemRef(reg_ref, rm_ref.mem);
    }
}

void Compiler::Impl::instr(
    uint8_t opcode,
    const Imm &imm,
    const RegRef &reg_ref)
{
    genREXPreffix(NOREG, NOREG, reg_ref);
    gen(static_cast<uint8_t>(opcode + (reg_ref.reg & ~R8.reg)));
    gen(imm);
}

void Compiler::Impl::instr(
    uint8_t opcode,
    uint8_t ext,
    const Imm &imm,
    const Ref &op1)
{
    instr(opcode, RegRef(static_cast<detail::DwordReg>(ext)), op1);
    gen(imm);
}

void Compiler::Impl::genREXPreffix(
    const RegRef &reg,
    const RegRef &index,
    const RegRef &base)
{
    uint8_t rex = c_rex;

    if (base.size == Size::Qword)
    {
        rex += c_rex_field_w;
    }

    if (base.reg >= R8.reg)
    {
        rex += c_rex_field_b;
    }

    if (index.reg >= R8.reg)
    {
        rex += c_rex_field_x;
    }

    if (reg.reg >= R8.reg)
    {
        rex += c_rex_field_r;
    }

    if (rex != c_rex)
    {
        gen(rex);
    }
}

void Compiler::Impl::genMemRef(const RegRef &reg_ref, const MemRef &mem_ref)
{
    int8_t mod;
    int8_t rm = mem_ref.base.reg;

    if (mem_ref.disp == 0)
    {
        mod = 0;
    }
    else if (isByte(mem_ref.disp))
    {
        mod = 1;
    }
    else
    {
        mod = 3;
    }

    if (mem_ref.base == NOREG && mem_ref.index == NOREG)
    {
        mod = 0;
        rm = 6;
    }

    composeByte(
        mod, reg_ref.reg & ~R8.reg, mem_ref.scale != 0 ? 5 : rm & ~R8.reg);

    if (mem_ref.scale != 0 || mem_ref.base.reg == 5)
    {
        genSIBByte(mem_ref);
    }

    if (mem_ref.disp != 0)
    {
        gen(isByte(mem_ref.disp) ? static_cast<int8_t>(mem_ref.disp)
                                 : mem_ref.disp);
    }
}

void Compiler::Impl::genSIBByte(const MemRef &mem_ref)
{
    // if (mem_ref.index.reg == 5)
    // {
    //     throw std::runtime_error("cannot index by %esp in SIB");
    // }

    composeByte(
        static_cast<int8_t>(log2(mem_ref.scale)),
        mem_ref.index.reg & ~R8.reg,
        mem_ref.base.reg & ~R8.reg);
}

void Compiler::Impl::composeByte(int8_t a, int8_t b, int8_t c)
{
    gen(static_cast<int8_t>((a << 6) + (b << 3) + c));
}

const ByteArray &Compiler::Impl::section(SectionID id) const
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

ByteArray &Compiler::Impl::section(SectionID id)
{
    return m_sections[id];
}

bool Compiler::Impl::isSectionDefined(SectionID id) const
{
    return m_sections.find(id) != m_sections.end();
}

std::size_t Compiler::Impl::sectionSize(SectionID id) const
{
    return isSectionDefined(id) ? section(id).size() : 0;
}

bool Compiler::Impl::isSymbolDefined(const std::string &name) const
{
    return m_symbols.find(name) != m_symbols.end();
}

void Compiler::Impl::pushSymbol(
    const std::string &name,
    const std::string &base_symbol,
    std::size_t offset)
{
    if (isSymbolDefined(name))
        throw std::runtime_error("symbol '" + name + "' is already defined");

    m_symbols[name] = Symbol{base_symbol, offset};
}

} // namespace x86_64
