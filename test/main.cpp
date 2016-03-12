#include "testmanager.h"

int main() {
    unit::TestManager::instance().run();

    return 0;
}
