#pragma once

#include "x86_64_compiler/bytearray.hpp"

namespace x86_64 {

class Function final
{
public: // methods
    explicit Function(const ByteArray &code);

    Function(const Function &f) = delete;
    Function(Function &&f) = delete;

    template <class R, class... Args>
    R invoke(Args &&... args) const;

private: // types
    template <class... Args>
    class InvokeHelper;

    template <class... Args>
    class InvokeHelper<void, Args...>
    {
    public:
        InvokeHelper(const Function &f_ref)
            : m_f_ref(f_ref)
        {
        }

        void operator()(Args &&... args) const
        {
            using FuncPtrType = void (*)(Args...);
            FuncPtrType f_ptr = reinterpret_cast<FuncPtrType>(m_f_ref.m_code);
            f_ptr(std::forward<Args>(args)...);
        }

    private:
        const Function &m_f_ref;
    };

    template <class R, class... Args>
    class InvokeHelper<R, Args...>
    {
    public:
        InvokeHelper(const Function &f_ref)
            : m_f_ref(f_ref)
        {
        }

        R operator()(Args &&... args) const
        {
            using FuncPtrType = R (*)(Args...);
            FuncPtrType f_ptr = reinterpret_cast<FuncPtrType>(m_f_ref.m_code);
            return f_ptr(std::forward<Args>(args)...);
        }

    private:
        const Function &m_f_ref;
    };

private: // methods
    void allocateMemory(const ByteArray &code);

private: // fields
    void *m_code;
};

template <class R, class... Args>
R Function::invoke(Args &&... args) const
{
    return InvokeHelper<R, Args...>(*this)(std::forward<Args>(args)...);
}

} // namespace x86_64
