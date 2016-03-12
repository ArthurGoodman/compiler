#include "addtest.h"

#include <iostream>

AddTest::AddTest()
    : TestSet("add") {
    test("test 1", "lel", []() {
        std::cout << "lel";
    });

    test("test 2", "lol", []() {
        std::cout << "not lol";
    });

    test("test 3", "kek", []() {
        std::cout << "kek";
    });
}
