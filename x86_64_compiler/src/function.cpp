#include "x86_64_compiler/function.hpp"

#include <cstring>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>

namespace x86_64 {

Function::Function(const ByteArray &code)
{
    allocateMemory(code);
}

void Function::allocateMemory(const ByteArray &code)
{
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1)
    {
        throw std::runtime_error("error in sysconf");
    }

    size_t size = static_cast<size_t>(page_size);

    m_code = memalign(size, size);
    if (m_code == nullptr)
    {
        throw std::runtime_error("error in memalign");
    }

    memcpy(m_code, code.data(), code.size());

    if (mprotect(m_code, size, PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
    {
        throw std::runtime_error("error in mprotect");
    }
}

} // namespace x86_64
