#include "gtest/gtest.h"

#include "TextInput/FileInputSource.h"
#include "pathutils.h"
#include <iostream>
#include "Lexer/Tokenizer.h"
#include "TextInput/StringInputSource.h"
#include "Parser/Parser.h"
#include "Runtime/Context.h"
#include "Runtime/CoreFunctions.h"
#include <Logger.h>
#include "TextInput/UnicodeConversion.h"

TEST(stringInputSource, eof_is_false) {
    auto stringInput = StringInputSource("関数、フィボナッチ（番号）");

    EXPECT_FALSE(stringInput.eof());
}

TEST(stringInputSource, readSingleCharacters) {
    auto stringInput = StringInputSource("関n数");

    EXPECT_EQ(stringInput.peekChar(), L'関');
    EXPECT_EQ(stringInput.peekChar(), L'関');
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
    auto filename = string("../example/fileInputSource.tnk");
    FileInputSource stringInput(filename.c_str());

    EXPECT_EQ(stringInput.peekChar(), L'関');
    EXPECT_EQ(stringInput.peekChar(), L'関');
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

void evalTanukiStarter(Environment *env) {
    auto source = StringInputSource(coreTanukiStarter);
    auto tokenizer = TanukiTokenizer(&source);
    auto parser = Parser(nullptr);
    SyntaxNode *tree = parser.run(tokenizer.getAllTokens());
    env->eval(tree);
}

TEST(stringInputSource, selftest) {
    auto filename = string("../testpin/tests.tnk");
    ConsoleLogger logger;
    ConsoleLogger::wide_mode = false;
    logger.logLn("Starting self test");
    FileInputSource stringInput(filename.c_str());
    auto t = TanukiTokenizer(&stringInput);
    auto p = Parser(nullptr);
    SyntaxNode *tree = p.run(t.getAllTokens());
    EXPECT_NE(tree->type, NodeType::PARSE_ERROR);
    Context context;
    // can reduce to further test garbage collection correctness
    context.setFrequency(100000);
    FilesystemImpl filesystem;
    auto *env = new Environment(&context, &filesystem);
    env->bind(
            "FILE",
            context.newStringValue(filename)
    );
    evalTanukiStarter(env);
    env->eval(tree);

    delete tree;
    context.cleanup();
}
