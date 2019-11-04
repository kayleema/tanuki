#include "Logger.h"
#include "gtest/gtest.h"
#include <iostream>
#include "wchar.h"

int main(int argc, wchar_t **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
