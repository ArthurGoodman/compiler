#include "x86_64_compiler/compiler.hpp"
#include <cmath>
#include <limits>
#include <map>

namespace x86_64 {

constexpr uint8_t c_mod_disp0 = 0;
constexpr uint8_t c_mod_disp8 = 1;
constexpr uint8_t c_mod_disp32 = 2;
constexpr uint8_t c_mod_reg = 3;

constexpr uint8_t c_x86_mask = 7;

constexpr uint8_t c_operand_size_override_prefix = 0x66;
constexpr uint8_t c_address_size_override_prefix = 0x67;

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

    MemRef mem(int64_t disp) const;
    MemRef mem(const RegRef &reg) const;
    MemRef mem(const RegRef &index, int8_t scale) const;
    MemRef mem(const RegRef &base, const RegRef &index, int8_t scale) const;

    SymRef abs(const std::string &name);
    SymRef rel(const std::string &name);

    void constant(uint8_t value);
    void constant(uint16_t value);
    void constant(uint32_t value);
    void constant(uint64_t value);
    void constant(double value);

    void call(int32_t disp);
    void callw(int16_t disp);
    void callq(int32_t disp);
    void call(const Ref &ref);
    void callw(const Ref &ref);
    void callq(const Ref &ref);
    void lcall(const Ref &ref);
    void lcallw(const Ref &ref);
    void lcalll(const Ref &ref);

    void mov(const Ref &src, const Ref &dst);
    void movb(uint8_t imm, const Ref &dst);
    void movw(uint16_t imm, const Ref &dst);
    void movl(uint32_t imm, const Ref &dst);
    void movq(uint64_t imm, const Ref &dst);

    void nop();

private: // methods
    void mov(const Imm &imm, const Ref &dst);

    void instr(uint8_t opcode, const Ref &op1, const Ref &dst);
    void instr(uint8_t opcode, int8_t reg, Size size, const Ref &rm_ref);
    void instr(uint8_t opcode, const Imm &imm, int8_t reg);
    void instr(uint8_t opcode, int8_t ext, const Imm &imm, const Ref &dst);

    void genREXPrefix(
        const int8_t &reg,
        Size size,
        const int8_t &index,
        const int8_t &base);

    void genRef(int8_t reg, const Ref &ref);
    void genRef(int8_t reg, const RegRef &reg_ref);
    void genRef(int8_t reg, const MemRef &mem_ref);
    void genSIB(const MemRef &mem_ref);

    void genCompositeByte(uint8_t a, uint8_t b, uint8_t c);

    template <class T>
    void gen(const T &value);

    void genb(uint8_t);
    void genw(uint16_t);
    void genl(uint32_t);
    void genq(uint64_t);

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
    typename std::enable_if<std::is_signed<T>::value, bool>::type isByte(
        T value)
    {
        return value >= INT8_MIN && value <= INT8_MAX;
    }

    template <class T>
    typename std::enable_if<!std::is_signed<T>::value, bool>::type isByte(
        T value)
    {
        return value <= INT8_MAX;
    }

    template <class T>
    typename std::enable_if<std::is_signed<T>::value, bool>::type isDword(
        T value)
    {
        return value >= INT32_MIN && value <= INT32_MAX;
    }

    template <class T>
    typename std::enable_if<!std::is_signed<T>::value, bool>::type isDword(
        T value)
    {
        return value <= INT32_MAX;
    }

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

bool detail::RegRef::operator==(const detail::RegRef &ref) const
{
    return size == ref.size && reg == ref.reg;
}

bool detail::RegRef::operator!=(const detail::RegRef &ref) const
{
    return !(*this == ref);
}

Compiler::MemRef::MemRef(int8_t scale, const RegRef &index, const RegRef &base)
    : scale{scale}
    , index{index}
    , base{base}
    , disp{0}
{
}

Compiler::MemRef Compiler::MemRef::operator-(int64_t offset) const
{
    return MemRef(*this, disp - offset);
}

Compiler::MemRef::MemRef(const MemRef &ref, int64_t disp)
    : scale{ref.scale}
    , index{ref.index}
    , base{ref.base}
    , disp{disp}
{
}

Compiler::MemRef Compiler::MemRef::operator+(int64_t offset) const
{
    return MemRef(*this, disp + offset);
}

Compiler::MemRef operator+(int64_t offset, const Compiler::MemRef &ref)
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

Compiler::SymRef Compiler::SymRef::operator+(int64_t offset) const
{
    return SymRef(*this, this->offset + offset);
}

Compiler::SymRef Compiler::SymRef::operator-(int64_t offset) const
{
    return SymRef(*this, this->offset - offset);
}

Compiler::SymRef::SymRef(const SymRef &ref, int64_t offset)
    : type{ref.type}
    , name{strdup(ref.name)}
    , offset{offset}
{
}

Compiler::SymRef operator+(int64_t offset, const Compiler::SymRef &ref)
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

Compiler::Ref Compiler::Ref::operator+(int64_t offset) const
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

Compiler::Ref operator+(int64_t offset, const Compiler::Ref &ref)
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

Compiler::MemRef Compiler::mem(int64_t disp) const
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

void Compiler::constant(uint64_t value)
{
    return m_impl->constant(value);
}

void Compiler::constant(double value)
{
    return m_impl->constant(value);
}

void Compiler::call(int32_t disp)
{
    return m_impl->call(disp);
}

void Compiler::callw(int16_t disp)
{
    return m_impl->callw(disp);
}

void Compiler::callq(int32_t disp)
{
    return m_impl->callq(disp);
}

void Compiler::call(const Compiler::Ref &ref)
{
    return m_impl->call(ref);
}

void Compiler::callw(const Compiler::Ref &ref)
{
    return m_impl->callw(ref);
}

void Compiler::callq(const Compiler::Ref &ref)
{
    return m_impl->callq(ref);
}

void Compiler::lcall(const Compiler::Ref &ref)
{
    return m_impl->lcall(ref);
}

void Compiler::lcallw(const Compiler::Ref &ref)
{
    return m_impl->lcallw(ref);
}

void Compiler::lcalll(const Compiler::Ref &ref)
{
    return m_impl->lcalll(ref);
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

void Compiler::nop()
{
    m_impl->nop();
}

Compiler::Impl::Imm::Imm(uint8_t value)
    : size{Size::Byte}
    , qword{value}
{
}

Compiler::Impl::Imm::Imm(uint16_t value)
    : size{Size::Word}
    , qword{value}
{
}

Compiler::Impl::Imm::Imm(uint32_t value)
    : size{Size::Dword}
    , qword{value}
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

Compiler::MemRef Compiler::Impl::mem(int64_t disp) const
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

void Compiler::Impl::constant(uint64_t value)
{
    gen(value);
}

void Compiler::Impl::constant(double value)
{
    gen(value);
}

void Compiler::Impl::call(int32_t disp)
{
    callq(disp);
}

void Compiler::Impl::callw(int16_t disp)
{
    ///@ hack: reg=0
    instr(0xe8, static_cast<uint16_t>(disp), 0);
}

void Compiler::Impl::callq(int32_t disp)
{
    ///@ hack: reg=0
    instr(0xe8, static_cast<uint32_t>(disp), 0);
}

void Compiler::Impl::call(const Ref &ref)
{
    callq(ref);
}

void Compiler::Impl::callw(const Ref &ref)
{
    ///@ hack: opcode=0xff-1
    instr(0xfe, 2, Size::Word, ref);
}

void Compiler::Impl::callq(const Ref &ref)
{
    ///@ hack: opcode=0xff-1
    instr(0xfe, 2, Size::Dword, ref);
}

void Compiler::Impl::lcall(const Ref &ref)
{
    lcalll(ref);
}

void Compiler::Impl::lcallw(const Ref &ref)
{
    ///@ hack: opcode=0xff-1
    instr(0xfe, 3, Size::Word, ref);
}

void Compiler::Impl::lcalll(const Ref &ref)
{
    ///@ hack: opcode=0xff-1
    instr(0xfe, 3, Size::Dword, ref);
}

void Compiler::Impl::mov(const Ref &src, const Ref &dst)
{
    if ((src.type == Ref::Type::Reg && src.reg.reg == 0 &&
         dst.type == Ref::Type::Mem && dst.mem.base == NOREG &&
         dst.mem.index == NOREG && !isDword(dst.mem.disp)) ||
        (dst.type == Ref::Type::Reg && dst.reg.reg == 0 &&
         src.type == Ref::Type::Mem && src.mem.base == NOREG &&
         src.mem.index == NOREG && !isDword(src.mem.disp)))
    {
        instr(0xa0, dst, src);
    }
    else
    {
        instr(0x88, src, dst);
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
    if (dst.type == Ref::Type::Mem ||
        (dst.reg.size == Size::Qword && isDword(imm.qword)))
    {
        instr(0xc6, 0, imm, dst);
    }
    else
    {
        instr(dst.reg.size == Size::Byte ? 0xb0 : 0xb8, imm, dst.reg.reg);
    }
}

void Compiler::Impl::nop()
{
    genb(0x90);
}

void Compiler::Impl::instr(uint8_t opcode, const Ref &src, const Ref &dst)
{
    RegRef reg_ref = src.type == Ref::Type::Reg ? src.reg : dst.reg;
    Ref rm_ref = src.type == Ref::Type::Reg ? dst : src;

    if (dst.type == Ref::Type::Reg && src.type != Ref::Type::Reg)
    {
        opcode += c_opcode_field_d;
    }

    instr(opcode, reg_ref.reg, reg_ref.size, rm_ref);
}

void Compiler::Impl::instr(
    uint8_t opcode,
    int8_t reg,
    Size size,
    const Ref &rm_ref)
{
    if (size != Size::Byte)
    {
        opcode += c_opcode_field_w;
    }

    if (rm_ref.type == Ref::Type::Mem &&
        ((rm_ref.mem.base != NOREG && rm_ref.mem.base.size != Size::Qword) ||
         (rm_ref.mem.index != NOREG && rm_ref.mem.index.size != Size::Qword)))
    {
        gen(c_address_size_override_prefix);
    }

    if (size == Size::Word)
    {
        gen(c_operand_size_override_prefix);
    }

    int8_t index = rm_ref.type == Ref::Type::Mem ? rm_ref.mem.index.reg : -1;
    int8_t base =
        rm_ref.type == Ref::Type::Mem ? rm_ref.mem.base.reg : rm_ref.reg.reg;

    genREXPrefix(reg, size, index, base);
    gen(opcode);
    genRef(reg, rm_ref);
}

void Compiler::Impl::instr(uint8_t opcode, const Imm &imm, int8_t reg)
{
    if (imm.size == Size::Word)
    {
        gen(c_operand_size_override_prefix);
    }

    genREXPrefix(-1, Size::None, -1, reg);
    genb(opcode + (reg & c_x86_mask));
    gen(imm);
}

void Compiler::Impl::instr(
    uint8_t opcode,
    int8_t ext,
    const Imm &imm,
    const Ref &dst)
{
    instr(opcode, ext, imm.size, dst);

    if (imm.size == Size::Qword)
    {
        gen(imm.dword);
    }
    else
    {
        gen(imm);
    }
}

void Compiler::Impl::genREXPrefix(
    const int8_t &reg,
    Size size,
    const int8_t &index,
    const int8_t &base)
{
    uint8_t rex = c_rex_field_w * (size == Size::Qword) +
                  c_rex_field_b * (base > c_x86_mask) +
                  c_rex_field_x * (index > c_x86_mask) +
                  c_rex_field_r * (reg > c_x86_mask);

    if (rex)
    {
        genb(c_rex + rex);
    }
}

void Compiler::Impl::genRef(int8_t reg, const Ref &ref)
{
    if (ref.type == Ref::Type::Reg)
    {
        genRef(reg, ref.reg);
    }
    else
    {
        genRef(reg, ref.mem);
    }
}

void Compiler::Impl::genRef(int8_t reg, const RegRef &reg_ref)
{
    genCompositeByte(c_mod_reg, reg & c_x86_mask, reg_ref.reg & c_x86_mask);
}

void Compiler::Impl::genRef(int8_t reg, const MemRef &mem_ref)
{
    if (!isDword(mem_ref.disp))
    {
        gen(mem_ref.disp);
        return;
    }

    uint8_t mod;
    uint8_t rm = mem_ref.base.reg & c_x86_mask;

    if ((mem_ref.disp == 0 && (mem_ref.base.reg & c_x86_mask) != 5) ||
        (mem_ref.scale && mem_ref.base == NOREG))
    {
        mod = c_mod_disp0;
    }
    else if (isByte(mem_ref.disp))
    {
        mod = c_mod_disp8;
    }
    else
    {
        mod = c_mod_disp32;
    }

    if (mem_ref.base == NOREG && mem_ref.index == NOREG)
    {
        mod = c_mod_disp0;
        rm = 4;
    }
    else if (mem_ref.scale)
    {
        rm = 4;
    }

    genCompositeByte(mod, reg & c_x86_mask, rm);
    genSIB(mem_ref);

    if (mem_ref.disp || (mem_ref.base.reg & c_x86_mask) == 5)
    {
        if (isByte(mem_ref.disp) && mem_ref.base != NOREG)
        {
            gen(static_cast<uint8_t>(mem_ref.disp));
        }
        else
        {
            gen(static_cast<uint32_t>(mem_ref.disp));
        }
    }
}

void Compiler::Impl::genSIB(const MemRef &mem_ref)
{
    int8_t scale = -1;
    uint8_t index = mem_ref.index.reg & c_x86_mask;
    uint8_t base = mem_ref.base.reg & c_x86_mask;

    if (mem_ref.scale)
    {
        if (mem_ref.index.reg == 4)
        {
            throw std::runtime_error("cannot index by %esp in SIB");
        }

        scale = mem_ref.scale;

        if (mem_ref.base == NOREG)
        {
            base = 5;
        }
    }
    else if (mem_ref.base == NOREG && mem_ref.index == NOREG)
    {
        scale = 0;
        index = 4;
        base = 5;
    }
    else if ((mem_ref.base.reg & c_x86_mask) == 4)
    {
        scale = 0;
        index = 4;
    }

    if (scale >= 0)
    {
        genCompositeByte(static_cast<uint8_t>(log2(scale)), index, base);
    }
}

void Compiler::Impl::genCompositeByte(uint8_t a, uint8_t b, uint8_t c)
{
    gen(static_cast<uint8_t>((a << 6) + (b << 3) + c));
}

void Compiler::Impl::genb(uint8_t value)
{
    gen(value);
}

void Compiler::Impl::genw(uint16_t value)
{
    gen(value);
}

void Compiler::Impl::genl(uint32_t value)
{
    gen(value);
}

void Compiler::Impl::genq(uint64_t value)
{
    gen(value);
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
