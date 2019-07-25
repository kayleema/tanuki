#include <iostream>

#include "gtest/gtest.h"

#include "Tokenizer.h"

TEST(tokenizer, simple_test) {
    auto stringInput = StringInputSource(L"関数、フィボナッチ（番号）");
    auto testTokenizer = FileTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::FUNC, L"関数", 1),
                    Token(TokenType::COMMA, L"、", 1),
                    Token(TokenType::SYMBOL, L"フィボナッチ", 1),
                    Token(TokenType::LPAREN, L"（", 1),
                    Token(TokenType::SYMBOL, L"番号", 1),
                    Token(TokenType::RPAREN, L"）", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, multiline) {
    auto stringInput = StringInputSource(
            L"関数、フィボナッチ（番号）\n"
            L"　返す、１２３"
    );
    auto testTokenizer = FileTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::NEWL, L"", 2),
                    Token(TokenType::INDENT, L"", 2),
                    Token(TokenType::RETURN, L"返す", 2),
                    Token(TokenType::COMMA, L"、", 2),
                    Token(TokenType::NUMBER, L"１２３", 2),
                    Token(TokenType::END, L"", 2),
            });
    EXPECT_EQ(vector<Token>(allTokens.begin() + 6, allTokens.end()), expected);
}

TEST(tokenizer, functions) {
    auto stringInput = StringInputSource(
            L"あ（い（）、う（））"
    );
    auto testTokenizer = FileTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, L"あ", 1),
                    Token(TokenType::LPAREN, L"（", 1),
                    Token(TokenType::SYMBOL, L"い", 1),
                    Token(TokenType::LPAREN, L"（", 1),
                    Token(TokenType::RPAREN, L"）", 1),
                    Token(TokenType::COMMA, L"、", 1),
                    Token(TokenType::SYMBOL, L"う", 1),
                    Token(TokenType::LPAREN, L"（", 1),
                    Token(TokenType::RPAREN, L"）", 1),
                    Token(TokenType::RPAREN, L"）", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, comments) {
    auto stringInput = StringInputSource(
            L"あ（）＃コメント\n"
            L"あ（＃コメント"
    );

    auto testTokenizer = FileTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, L"あ", 1),
                    Token(TokenType::LPAREN, L"（", 1),
                    Token(TokenType::RPAREN, L"）", 1),
                    Token(TokenType::NEWL, L"", 2),

                    Token(TokenType::SYMBOL, L"あ", 2),
                    Token(TokenType::LPAREN, L"（", 2),
                    Token(TokenType::END, L"", 2),
            });
    EXPECT_EQ(allTokens, expected);
}
