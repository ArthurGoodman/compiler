#include "common.h"

uint ceilToPowerof2(uint n) {
    return 1 << (sizeof(n) * 8 - __builtin_clz(n - 1));
}
