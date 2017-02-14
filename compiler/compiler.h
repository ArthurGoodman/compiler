#pragma once

#include "function.h"

#include <map>
#include <cstring>

namespace x86 {

enum Register {
    EAX,
    ECX,
    EDX,
    EBX,
    ESP,
    EBP,
    ESI,
    EDI
};

class Compiler {
    struct __attribute__((packed)) DosHeader {
        uint16_t magic;
        uint16_t usedBytesInTheLastPage;
        uint16_t fileSizeInPages;
        uint16_t numberOfRelocationItems;
        uint16_t headerSizeInParagraphs;
        uint16_t minimumExtraParagraphs;
        uint16_t maximumExtraParagraphs;
        uint16_t initialRelativeSS;
        uint16_t initialSP;
        uint16_t checksum;
        uint16_t initialIP;
        uint16_t initialRelativeCS;
        uint16_t addressOfRelocationTable;
        uint16_t overlayNumber;
        uint16_t reserved[4];
        uint16_t OEMid;
        uint16_t OEMinfo;
        uint16_t reserved2[10];
        uint32_t addressOfNewExeHeader;
    };

    struct __attribute__((packed)) FileHeader {
        uint16_t machine;
        uint16_t numberOfSections;
        uint32_t timeDateStamp;
        uint32_t pointerToSymbolTable;
        uint32_t numberOfSymbols;
        uint16_t sizeOfOptionalHeader;
        uint16_t characteristics;
    };

    enum MachineTypes {
        MT_Invalid = 0xffff,

        IMAGE_FILE_MACHINE_UNKNOWN = 0x0,
        IMAGE_FILE_MACHINE_AM33 = 0x13,
        IMAGE_FILE_MACHINE_AMD64 = 0x8664,
        IMAGE_FILE_MACHINE_ARM = 0x1C0,
        IMAGE_FILE_MACHINE_ARMNT = 0x1C4,
        IMAGE_FILE_MACHINE_ARM64 = 0xAA64,
        IMAGE_FILE_MACHINE_EBC = 0xEBC,
        IMAGE_FILE_MACHINE_I386 = 0x14C,
        IMAGE_FILE_MACHINE_IA64 = 0x200,
        IMAGE_FILE_MACHINE_M32R = 0x9041,
        IMAGE_FILE_MACHINE_MIPS16 = 0x266,
        IMAGE_FILE_MACHINE_MIPSFPU = 0x366,
        IMAGE_FILE_MACHINE_MIPSFPU16 = 0x466,
        IMAGE_FILE_MACHINE_POWERPC = 0x1F0,
        IMAGE_FILE_MACHINE_POWERPCFP = 0x1F1,
        IMAGE_FILE_MACHINE_R4000 = 0x166,
        IMAGE_FILE_MACHINE_SH3 = 0x1A2,
        IMAGE_FILE_MACHINE_SH3DSP = 0x1A3,
        IMAGE_FILE_MACHINE_SH4 = 0x1A6,
        IMAGE_FILE_MACHINE_SH5 = 0x1A8,
        IMAGE_FILE_MACHINE_THUMB = 0x1C2,
        IMAGE_FILE_MACHINE_WCEMIPSV2 = 0x169
    };

    enum Characteristics {
        C_Invalid = 0,

        IMAGE_FILE_RELOCS_STRIPPED = 0x0001,         /// The file does not contain base relocations and must be loaded at its preferred base. If this cannot be done, the loader will error.
        IMAGE_FILE_EXECUTABLE_IMAGE = 0x0002,        /// The file is valid and can be run.
        IMAGE_FILE_LINE_NUMS_STRIPPED = 0x0004,      /// COFF line numbers have been stripped. This is deprecated and should be 0.
        IMAGE_FILE_LOCAL_SYMS_STRIPPED = 0x0008,     /// COFF symbol table entries for local symbols have been removed. This is deprecated and should be 0.
        IMAGE_FILE_AGGRESSIVE_WS_TRIM = 0x0010,      /// Aggressively trim working set. This is deprecated and must be 0.
        IMAGE_FILE_LARGE_ADDRESS_AWARE = 0x0020,     /// Image can handle > 2GiB addresses.
        IMAGE_FILE_BYTES_REVERSED_LO = 0x0080,       /// Little endian: the LSB precedes the MSB in memory. This is deprecated and should be 0.
        IMAGE_FILE_32BIT_MACHINE = 0x0100,           /// Machine is based on a 32bit word architecture.
        IMAGE_FILE_DEBUG_STRIPPED = 0x0200,          /// Debugging info has been removed.
        IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP = 0x0400, /// If the image is on removable media, fully load it and copy it to swap.
        IMAGE_FILE_NET_RUN_FROM_SWAP = 0x0800,       /// If the image is on network media, fully load it and copy it to swap.
        IMAGE_FILE_SYSTEM = 0x1000,                  /// The image file is a system file, not a user program.
        IMAGE_FILE_DLL = 0x2000,                     /// The image file is a DLL.
        IMAGE_FILE_UP_SYSTEM_ONLY = 0x4000,          /// This file should only be run on a uniprocessor machine.
        IMAGE_FILE_BYTES_REVERSED_HI = 0x8000        /// Big endian: the MSB precedes the LSB in memory. This is deprecated and should be 0.
    };

    struct __attribute__((packed)) DataDirectory {
        uint32_t virtualAddress;
        uint32_t size;
    };

    enum DataDirectoryIndex {
        EXPORT_TABLE = 0,
        IMPORT_TABLE,
        RESOURCE_TABLE,
        EXCEPTION_TABLE,
        CERTIFICATE_TABLE,
        BASE_RELOCATION_TABLE,
        DEBUG,
        ARCHITECTURE,
        GLOBAL_PTR,
        TLS_TABLE,
        LOAD_CONFIG_TABLE,
        BOUND_IMPORT,
        IAT,
        DELAY_IMPORT_DESCRIPTOR,
        CLR_RUNTIME_HEADER,
        RESERVED,

        NUM_DATA_DIRECTORIES
    };

    enum WindowsSubsystem {
        IMAGE_SUBSYSTEM_UNKNOWN = 0,                  // An unknown subsystem.
        IMAGE_SUBSYSTEM_NATIVE = 1,                   // Device drivers and native Windows processes
        IMAGE_SUBSYSTEM_WINDOWS_GUI = 2,              // The Windows GUI subsystem.
        IMAGE_SUBSYSTEM_WINDOWS_CUI = 3,              // The Windows character subsystem.
        IMAGE_SUBSYSTEM_OS2_CUI = 5,                  // The OS/2 character subsytem.
        IMAGE_SUBSYSTEM_POSIX_CUI = 7,                // The POSIX character subsystem.
        IMAGE_SUBSYSTEM_NATIVE_WINDOWS = 8,           // Native Windows 9x driver.
        IMAGE_SUBSYSTEM_WINDOWS_CE_GUI = 9,           // Windows CE.
        IMAGE_SUBSYSTEM_EFI_APPLICATION = 10,         // An EFI application.
        IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER = 11, // An EFI driver with boot services.
        IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER = 12,      // An EFI driver with run-time services.
        IMAGE_SUBSYSTEM_EFI_ROM = 13,                 // An EFI ROM image.
        IMAGE_SUBSYSTEM_XBOX = 14,                    // XBOX.
        IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION = 16 // A BCD application.
    };

    enum DLLCharacteristics {
        IMAGE_DLL_CHARACTERISTICS_HIGH_ENTROPY_VA = 0x0020,      // ASLR with 64 bit address space.
        IMAGE_DLL_CHARACTERISTICS_DYNAMIC_BASE = 0x0040,         // DLL can be relocated at load time.
        IMAGE_DLL_CHARACTERISTICS_FORCE_INTEGRITY = 0x0080,      // Code integrity checks are enforced.
        IMAGE_DLL_CHARACTERISTICS_NX_COMPAT = 0x0100,            // Image is NX compatible.
        IMAGE_DLL_CHARACTERISTICS_NO_ISOLATION = 0x0200,         // Isolation aware, but do not isolate the image.
        IMAGE_DLL_CHARACTERISTICS_NO_SEH = 0x0400,               // Does not use structured exception handling (SEH). No SEH handler may be called in this image.
        IMAGE_DLL_CHARACTERISTICS_NO_BIND = 0x0800,              // Do not bind the image.
        IMAGE_DLL_CHARACTERISTICS_APPCONTAINER = 0x1000,         // Image should execute in an AppContainer.
        IMAGE_DLL_CHARACTERISTICS_WDM_DRIVER = 0x2000,           // A WDM driver.
        IMAGE_DLL_CHARACTERISTICS_GUARD_CF = 0x4000,             // Image supports Control Flow Guard.
        IMAGE_DLL_CHARACTERISTICS_TERMINAL_SERVER_AWARE = 0x8000 // Terminal Server aware.
    };

    struct __attribute__((packed)) OptionalHeader {
        uint16_t magic;
        uint8_t majorLinkerVersion;
        uint8_t minorLinkerVersion;
        uint32_t sizeOfCode;
        uint32_t sizeOfInitializedData;
        uint32_t sizeOfUninitializedData;
        uint32_t addressOfEntryPoint;
        uint32_t baseOfCode;
        uint32_t baseOfData;
        uint32_t imageBase;
        uint32_t sectionAlignment;
        uint32_t fileAlignment;
        uint16_t majorOperatingSystemVersion;
        uint16_t minorOperatingSystemVersion;
        uint16_t majorImageVersion;
        uint16_t minorImageVersion;
        uint16_t majorSubsystemVersion;
        uint16_t minorSubsystemVersion;
        uint32_t win32VersionValue;
        uint32_t sizeOfImage;
        uint32_t sizeOfHeaders;
        uint32_t checkSum;
        uint16_t subsystem;
        uint16_t dllCharacteristics;
        uint32_t sizeOfStackReserve;
        uint32_t sizeOfStackCommit;
        uint32_t sizeOfHeapReserve;
        uint32_t sizeOfHeapCommit;
        uint32_t loaderFlags;
        uint32_t numberOfRvaAndSizes;
        DataDirectory dataDirectory[NUM_DATA_DIRECTORIES];
    };

    struct __attribute__((packed)) NTHeaders {
        uint32_t signature;
        FileHeader fileHeader;
        OptionalHeader optionalHeader;
    };

    struct __attribute__((packed)) SectionHeader {
        char name[8];
        uint32_t virtualSize;
        uint32_t virtualAddress;
        uint32_t sizeOfRawData;
        uint32_t pointerToRawData;
        uint32_t pointerToRelocations;
        uint32_t pointerToLinenumbers;
        uint16_t numberOfRelocations;
        uint16_t numberOfLinenumbers;
        uint32_t characteristics;
    };

    enum SectionCharacteristics : uint32_t {
        SC_Invalid = 0xffffffff,

        IMAGE_SCN_TYPE_NOLOAD = 0x00000002,
        IMAGE_SCN_TYPE_NO_PAD = 0x00000008,
        IMAGE_SCN_CNT_CODE = 0x00000020,
        IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040,
        IMAGE_SCN_CNT_UNINITIALIZED_DATA = 0x00000080,
        IMAGE_SCN_LNK_OTHER = 0x00000100,
        IMAGE_SCN_LNK_INFO = 0x00000200,
        IMAGE_SCN_LNK_REMOVE = 0x00000800,
        IMAGE_SCN_LNK_COMDAT = 0x00001000,
        IMAGE_SCN_GPREL = 0x00008000,
        IMAGE_SCN_MEM_PURGEABLE = 0x00020000,
        IMAGE_SCN_MEM_16BIT = 0x00020000,
        IMAGE_SCN_MEM_LOCKED = 0x00040000,
        IMAGE_SCN_MEM_PRELOAD = 0x00080000,
        IMAGE_SCN_ALIGN_1BYTES = 0x00100000,
        IMAGE_SCN_ALIGN_2BYTES = 0x00200000,
        IMAGE_SCN_ALIGN_4BYTES = 0x00300000,
        IMAGE_SCN_ALIGN_8BYTES = 0x00400000,
        IMAGE_SCN_ALIGN_16BYTES = 0x00500000,
        IMAGE_SCN_ALIGN_32BYTES = 0x00600000,
        IMAGE_SCN_ALIGN_64BYTES = 0x00700000,
        IMAGE_SCN_ALIGN_128BYTES = 0x00800000,
        IMAGE_SCN_ALIGN_256BYTES = 0x00900000,
        IMAGE_SCN_ALIGN_512BYTES = 0x00A00000,
        IMAGE_SCN_ALIGN_1024BYTES = 0x00B00000,
        IMAGE_SCN_ALIGN_2048BYTES = 0x00C00000,
        IMAGE_SCN_ALIGN_4096BYTES = 0x00D00000,
        IMAGE_SCN_ALIGN_8192BYTES = 0x00E00000,
        IMAGE_SCN_LNK_NRELOC_OVFL = 0x01000000,
        IMAGE_SCN_MEM_DISCARDABLE = 0x02000000,
        IMAGE_SCN_MEM_NOT_CACHED = 0x04000000,
        IMAGE_SCN_MEM_NOT_PAGED = 0x08000000,
        IMAGE_SCN_MEM_SHARED = 0x10000000,
        IMAGE_SCN_MEM_EXECUTE = 0x20000000,
        IMAGE_SCN_MEM_READ = 0x40000000,
        IMAGE_SCN_MEM_WRITE = 0x80000000
    };

    struct __attribute__((packed)) RelocationDirective {
        uint32_t virtualAddress;
        uint32_t symbolIndex;
        uint16_t type;
    };

    enum RelocationTypeI386 {
        IMAGE_REL_I386_ABSOLUTE = 0x0000,
        IMAGE_REL_I386_DIR16 = 0x0001,
        IMAGE_REL_I386_REL16 = 0x0002,
        IMAGE_REL_I386_DIR32 = 0x0006,
        IMAGE_REL_I386_DIR32NB = 0x0007,
        IMAGE_REL_I386_SEG12 = 0x0009,
        IMAGE_REL_I386_SECTION = 0x000A,
        IMAGE_REL_I386_SECREL = 0x000B,
        IMAGE_REL_I386_TOKEN = 0x000C,
        IMAGE_REL_I386_SECREL7 = 0x000D,
        IMAGE_REL_I386_REL32 = 0x0014
    };

    struct __attribute__((packed)) ExportDirectory {
        uint32_t characteristics;
        uint32_t timeDateStamp;
        uint16_t majorVersion;
        uint16_t minorVersion;
        uint32_t name;
        uint32_t base;
        uint32_t numberOfFunctions;
        uint32_t numberOfNames;
        uint32_t addressOfFunctions;
        uint32_t addressOfNames;
        uint32_t addressOfNameOrdinals;
    };

    struct __attribute__((packed)) ImportDescriptor {
        uint32_t originalFirstThunk;
        uint32_t timeDateStamp;
        uint32_t forwarderChain;
        uint32_t name;
        uint32_t firstThunk;
    };

    struct __attribute__((packed)) SymbolTableEntry {
        union {
            char name[8];
            struct {
                uint32_t zeroes;
                uint32_t offset;
            } e;
        } e;
        uint32_t value;
        int16_t sectionNumber;
        uint16_t type;
        uint8_t storageClass;
        uint8_t numberOfAuxSymbols;
    };

    enum SymbolSectionNumber : int16_t {
        IMAGE_SYM_DEBUG = -2,
        IMAGE_SYM_ABSOLUTE = -1,
        IMAGE_SYM_UNDEFINED = 0
    };

    enum SymbolStorageClass {
        SSC_Invalid = 0xff,

        IMAGE_SYM_CLASS_END_OF_FUNCTION = -1,  // Physical end of function
        IMAGE_SYM_CLASS_NULL = 0,              // No symbol
        IMAGE_SYM_CLASS_AUTOMATIC = 1,         // Stack variable
        IMAGE_SYM_CLASS_EXTERNAL = 2,          // External symbol
        IMAGE_SYM_CLASS_STATIC = 3,            // Static
        IMAGE_SYM_CLASS_REGISTER = 4,          // Register variable
        IMAGE_SYM_CLASS_EXTERNAL_DEF = 5,      // External definition
        IMAGE_SYM_CLASS_LABEL = 6,             // Label
        IMAGE_SYM_CLASS_UNDEFINED_LABEL = 7,   // Undefined label
        IMAGE_SYM_CLASS_MEMBER_OF_STRUCT = 8,  // Member of structure
        IMAGE_SYM_CLASS_ARGUMENT = 9,          // Function argument
        IMAGE_SYM_CLASS_STRUCT_TAG = 10,       // Structure tag
        IMAGE_SYM_CLASS_MEMBER_OF_UNION = 11,  // Member of union
        IMAGE_SYM_CLASS_UNION_TAG = 12,        // Union tag
        IMAGE_SYM_CLASS_TYPE_DEFINITION = 13,  // Type definition
        IMAGE_SYM_CLASS_UNDEFINED_STATIC = 14, // Undefined static
        IMAGE_SYM_CLASS_ENUM_TAG = 15,         // Enumeration tag
        IMAGE_SYM_CLASS_MEMBER_OF_ENUM = 16,   // Member of enumeration
        IMAGE_SYM_CLASS_REGISTER_PARAM = 17,   // Register parameter
        IMAGE_SYM_CLASS_BIT_FIELD = 18,        // Bit field
        IMAGE_SYM_CLASS_BLOCK = 100,           // ".bb" or ".eb" - beginning or end of block
        IMAGE_SYM_CLASS_FUNCTION = 101,        // ".bf" or ".ef" - beginning or end of function
        IMAGE_SYM_CLASS_END_OF_STRUCT = 102,   // End of structure
        IMAGE_SYM_CLASS_FILE = 103,            // File name
        IMAGE_SYM_CLASS_SECTION = 104,         // Line number, reformatted as symbol
        IMAGE_SYM_CLASS_WEAK_EXTERNAL = 105,   // Duplicate tag
        IMAGE_SYM_CLASS_CLR_TOKEN = 107        // External symbol in dmert public lib
    };

    enum SymbolBaseType {
        IMAGE_SYM_TYPE_NULL = 0,   // No type information or unknown base type.
        IMAGE_SYM_TYPE_VOID = 1,   // Used with void pointers and functions.
        IMAGE_SYM_TYPE_CHAR = 2,   // A character (signed byte).
        IMAGE_SYM_TYPE_SHORT = 3,  // A 2-byte signed integer.
        IMAGE_SYM_TYPE_INT = 4,    // A natural integer type on the target.
        IMAGE_SYM_TYPE_LONG = 5,   // A 4-byte signed integer.
        IMAGE_SYM_TYPE_FLOAT = 6,  // A 4-byte floating-point number.
        IMAGE_SYM_TYPE_DOUBLE = 7, // An 8-byte floating-point number.
        IMAGE_SYM_TYPE_STRUCT = 8, // A structure.
        IMAGE_SYM_TYPE_UNION = 9,  // An union.
        IMAGE_SYM_TYPE_ENUM = 10,  // An enumerated type.
        IMAGE_SYM_TYPE_MOE = 11,   // A member of enumeration (a specific value).
        IMAGE_SYM_TYPE_BYTE = 12,  // A byte; unsigned 1-byte integer.
        IMAGE_SYM_TYPE_WORD = 13,  // A word; unsigned 2-byte integer.
        IMAGE_SYM_TYPE_UINT = 14,  // An unsigned integer of natural size.
        IMAGE_SYM_TYPE_DWORD = 15  // An unsigned 4-byte integer.
    };

    enum SymbolComplexType {
        IMAGE_SYM_DTYPE_NULL = 0,     // No complex type; simple scalar variable.
        IMAGE_SYM_DTYPE_POINTER = 1,  // A pointer to base type.
        IMAGE_SYM_DTYPE_FUNCTION = 2, // A function that returns a base type.
        IMAGE_SYM_DTYPE_ARRAY = 3,    // An array of base type.

        SCT_COMPLEX_TYPE_SHIFT = 4 // Type is formed as (base + (derived << SCT_COMPLEX_TYPE_SHIFT))
    };

    struct __attribute__((packed)) ImageBaseRelocation {
        uint32_t virtualAddress;
        uint32_t sizeOfBlock;
    };

    enum SectionID {
        TEXT = 1,
        DATA,
        BSS,
        RDATA,
        EDATA,
        IDATA,
        RELOC
    };

    struct Symbol {
        std::string baseSymbol;
        uint offset;
    };

    enum SymRefType {
        RefAbs,
        RefRel
    };

    struct SymRef {
        std::string name;
        SymRefType type;
        int offset;

        SymRef operator+(int offset) const;
    };

    struct Reloc {
        std::string name;
        SymRefType type;
        uint offset;
    };

    struct MemRef {
        byte mod;
        byte rm;
        byte scale;
        byte index;
        byte base;
        int disp;
    };

    std::map<SectionID, ByteArray> sections;

    std::vector<std::string> exports;
    std::map<std::string, std::vector<std::string>> imports;
    std::map<std::string, Symbol> symbols;
    std::vector<Reloc> relocs;

    std::vector<std::string> funcs;
    std::vector<std::string> sectionNames;
    std::vector<std::string> externFuncs;
    std::vector<std::string> externVars;

    enum Mod {
        Disp0,
        Disp8,
        Disp32,
        Reg
    };

public:
    void rdata(const std::string &name, const byte *data, uint size);

    template <class T>
    void rdata(const std::string &name, T data);

    void data(const std::string &name, const byte *data, uint size);

    template <class T>
    void data(const std::string &name, T data);

    void bss(const std::string &name, uint size);

    void externalFunction(const std::string &name);
    void externalVariable(const std::string &name);

    void function(const std::string &name);

    SymRef abs(const std::string &name) const;
    SymRef rel(const std::string &name) const;

    MemRef ref(Register reg) const;
    MemRef ref(byte disp, Register reg) const;
    MemRef ref(int disp, Register reg) const;
    MemRef ref(int disp) const;
    MemRef ref(Register base, Register index, byte scale) const;
    MemRef ref(byte disp, Register base, Register index, byte scale) const;
    MemRef ref(int disp, Register base, Register index, byte scale) const;
    MemRef ref(int disp, Register index, byte scale) const;
    MemRef ref(Register index, byte scale) const;

    void push(Register reg);
    void push(const MemRef &ref);
    void push(byte value);
    void push(int value);
    void push(const SymRef &ref);

    void pop(Register reg);
    void pop(const MemRef &ref);

    void call(int disp);
    void call(const SymRef &ref);
    void call(Register reg);
    void call(const MemRef &ref);

    void mov(Register src, Register dst);
    void mov(int imm, Register dst);
    void mov(const SymRef &ref, Register dst);
    void mov(int imm, const MemRef &dst);
    void mov(const SymRef &src, const MemRef &dst);
    void mov(Register src, const MemRef &dst);
    void mov(const MemRef &src, Register dst);

    void lea(const MemRef &src, Register dst);

    void add(byte imm, Register dst);
    void add(int imm, Register dst);
    void add(byte imm, const MemRef &dst);
    void addb(byte imm, const MemRef &dst);
    void add(int imm, const MemRef &dst);
    void add(Register src, const MemRef &dst);
    void add(const MemRef &src, Register dst);

    void sub(byte imm, Register dst);
    void sub(int imm, Register dst);
    void sub(byte imm, const MemRef &dst);
    void subb(byte imm, const MemRef &dst);
    void sub(int imm, const MemRef &dst);
    void sub(Register src, const MemRef &dst);
    void sub(const MemRef &src, Register dst);

    void leave();
    void ret();

    void nop();

    ByteArray writeOBJ() const;
    ByteArray writeEXE() const;
    ByteArray writeDLL(const std::string &name) const;

private:
    void instr(byte op);
    void instr(byte op, byte imm);
    void instr(byte op, int imm);
    void instr(byte op, const SymRef &ref);
    void instr(byte op, byte reg, Register rm);
    void instr(byte op, byte reg, Register rm, byte imm);
    void instr(byte op, byte reg, Register rm, int imm);
    void instr(byte op, byte reg, Register rm, const SymRef &ref);
    void instr(byte op, byte reg, const MemRef &rm);
    void instr(byte op, byte reg, const MemRef &rm, byte imm);
    void instr(byte op, byte reg, const MemRef &rm, int imm);
    void instr(byte op, byte reg, const MemRef &rm, const SymRef &ref);

    static byte composeByte(byte a, byte b, byte c);

    template <class T>
    void gen(T value);

    bool isSectionDefined(SectionID id) const;
    uint sectionSize(SectionID id) const;
    ByteArray &section(SectionID id);
    const ByteArray &section(SectionID id) const;

    bool isSymbolDefined(const std::string &name) const;
    void pushSymbol(const std::string &name, const std::string &baseSymbol, uint offset);
    void pushReloc(const Reloc &reloc);

    //    static const char *sectionIDToName(SectionID id);
};

inline byte Compiler::composeByte(byte a, byte b, byte c) {
    return (byte)(a << 6 | b << 3 | c);
}

template <class T>
inline void Compiler::gen(T value) {
    section(TEXT).push(value);
}

template <class T>
inline void Compiler::rdata(const std::string &name, T data) {
    rdata(name, (const byte *)&data, sizeof(data));
}

template <>
inline void Compiler::rdata(const std::string &name, const char *data) {
    rdata(name, (const byte *)data, strlen(data));
}

template <class T>
inline void Compiler::data(const std::string &name, T data) {
    Compiler::data(name, (const byte *)&data, sizeof(data));
}

template <>
inline void Compiler::data(const std::string &name, const char *data) {
    Compiler::data(name, (const byte *)data, strlen(data));
}
}
