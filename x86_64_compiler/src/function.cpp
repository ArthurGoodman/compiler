#include "x86_64_compiler/function.hpp"

#include <cstring>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>

namespace x86_64 {

Function::Function(const ByteArray &code)
{
    allocateMemory(code.data(), code.size());
}

Function::Function(const Function &f)
    : m_code_ptr{nullptr}
{
    *this = f;
}

Function::Function(Function &&f)
    : m_code_ptr{nullptr}
{
    *this = std::move(f);
}

Function::~Function()
{
    free(m_code_ptr);
}

Function &Function::operator=(const Function &f)
{
    free(m_code_ptr);
    allocateMemory(f.m_code_ptr, f.m_code_size);

    return *this;
}

Function &Function::operator=(Function &&f)
{
    free(m_code_ptr);

    m_code_ptr = f.m_code_ptr;
    m_code_size = f.m_code_size;

    f.m_code_ptr = nullptr;
    f.m_code_size = 0;

    return *this;
}

void Function::allocateMemory(const void *code_ptr, size_t code_size)
{
    long page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1)
    {
        throw std::runtime_error("error in sysconf");
    }

    size_t size = static_cast<size_t>(page_size);

    m_code_ptr = memalign(size, size);
    if (m_code_ptr == nullptr)
    {
        throw std::runtime_error("error in memalign");
    }

    memcpy(m_code_ptr, code_ptr, code_size);
    m_code_size = code_size;

    if (mprotect(m_code_ptr, size, PROT_READ | PROT_WRITE | PROT_EXEC) == -1)
    {
        throw std::runtime_error("error in mprotect");
    }
}

} // namespace x86_64
