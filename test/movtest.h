#pragma once

#include "testset.h"
#include "compiler.h"

class MovTest : public unit::TestSet {
    vm::Compiler c;

public:
    MovTest();

private:
    void dump();
};
