#pragma once

#include "testset.h"
#include "compiler.h"

class CompilerTest : public unit::TestSet {
protected:
    vm::Compiler c;

public:
    CompilerTest(const std::string &name);

    void $(const std::string &name, const std::string &result, const std::function<void()> &f);
};