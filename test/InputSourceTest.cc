#include "gtest/gtest.h"

#include "InputSource.h"
#include "pathutils.h"
#include <iostream>

TEST(stringInputSource, eof_is_false) {
    auto stringInput = StringInputSource(L"関数、フィボナッチ（番号）");

    EXPECT_FALSE(stringInput.eof());
}

TEST(stringInputSource, readSingleCharacters) {
    auto stringInput = StringInputSource(L"関n数");

    EXPECT_EQ(stringInput.peekChar(), L'関');
    EXPECT_EQ(stringInput.getChar(), L'関');
    EXPECT_EQ(stringInput.peekChar(), L'n');
    EXPECT_EQ(stringInput.getChar(), L'n');
    EXPECT_EQ(stringInput.peekChar(), L'数');
    EXPECT_FALSE(stringInput.eof());

    // 最後の文字
    EXPECT_EQ(stringInput.getChar(), L'数');
    EXPECT_FALSE(stringInput.eof());

    // 終わり
    EXPECT_EQ(stringInput.peekChar(), L'\0');
    EXPECT_EQ(stringInput.getChar(), L'\0');
    EXPECT_TRUE(stringInput.eof());

    // 全部読んだ後
    EXPECT_EQ(stringInput.peekChar(), L'\0');
    EXPECT_EQ(stringInput.getChar(), L'\0');
    EXPECT_TRUE(stringInput.eof());
}

TEST(stringInputSource, fileSource) {
    auto filename = string("example/fileInputSource.pin");
    FileInputSource stringInput(filename.c_str());

    EXPECT_EQ(stringInput.peekChar(), L'関');
    EXPECT_EQ(stringInput.getChar(), L'関');
    EXPECT_EQ(stringInput.peekChar(), L'n');
    EXPECT_EQ(stringInput.getChar(), L'n');
    EXPECT_EQ(stringInput.peekChar(), L'数');
    EXPECT_FALSE(stringInput.eof());

    // 最後の文字
    EXPECT_EQ(stringInput.getChar(), L'数');
    EXPECT_FALSE(stringInput.eof());

    // 終わり
    EXPECT_EQ(stringInput.peekChar(), L'\0');
    EXPECT_EQ(stringInput.getChar(), L'\0');
    EXPECT_TRUE(stringInput.eof());

    // 全部読んだ後
    EXPECT_EQ(stringInput.peekChar(), L'\0');
    EXPECT_EQ(stringInput.getChar(), L'\0');
    EXPECT_TRUE(stringInput.eof());
}
