#pragma once

#include "testset.h"
#include "compiler.h"

class AddTest : public unit::TestSet {
    vm::Compiler c;

public:
    AddTest();

private:
    void dump();
};
