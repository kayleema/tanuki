#include "Logger.h"
#include "gtest/gtest.h"
#include <iostream>
#include "wchar.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
