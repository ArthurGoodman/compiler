#pragma once

#include "testset.h"
#include "compiler.h"

class PushPopTest : public unit::TestSet {
    vm::Compiler c;

public:
    PushPopTest();

private:
    void dump();
};
