#include <iostream>

#include "gtest/gtest.h"

#include "Lexer/Tokenizer.h"

TEST(tokenizer, simple_test) {
    auto stringInput = StringInputSource(L"関数、フィボナッチ（番号）");
    auto testTokenizer = InputSourceTokenizer(&stringInput);

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
    auto testTokenizer = InputSourceTokenizer(&stringInput);

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

TEST(tokenizer, emptyline) {
    auto stringInput = StringInputSource(L"関数、フィボナッチ（番号）\n"
                                         L"　返す、１２３\n"
                                         L"\n"
                                         L"フィボナッチ（４）");
    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    EXPECT_EQ(tokenListToTypeList(allTokens),
              "function, comma, symbol, lparen, symbol, rparen, newl, "
              "indent, return, comma, number, newl, "
              "dedent, newl, "
              "symbol, lparen, number, rparen, end");
}

TEST(tokenizer, parsesFloats) {
    auto stringInput = StringInputSource(
            L"関数、フィボナッチ（番号）\n"
            L"　返す、１２３。４"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::NEWL, L"", 2),
                    Token(TokenType::INDENT, L"", 2),
                    Token(TokenType::RETURN, L"返す", 2),
                    Token(TokenType::COMMA, L"、", 2),
                    Token(TokenType::NUMBER_FLOAT, L"１２３。４", 2),
                    Token(TokenType::END, L"", 2),
            });
    EXPECT_EQ(vector<Token>(allTokens.begin() + 6, allTokens.end()), expected);
}

TEST(tokenizer, functions) {
    auto stringInput = StringInputSource(
            L"あ（い（）、う（））"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);

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

    auto testTokenizer = InputSourceTokenizer(&stringInput);

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

TEST(tokenizer, indentation) {
    auto stringInput = StringInputSource(
            L"　　あ\n"
            L"　　　い\n"
            L"　　　　う\n"
            L"　　　い\n"
            L"　　　　う\n"
            L"　　あ\n"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::INDENT, L"", 1),
                    Token(TokenType::INDENT, L"", 1),

                    Token(TokenType::SYMBOL, L"あ", 1),
                    Token(TokenType::NEWL, L"", 2),
                    Token(TokenType::INDENT, L"", 2),

                    Token(TokenType::SYMBOL, L"い", 2),
                    Token(TokenType::NEWL, L"", 3),
                    Token(TokenType::INDENT, L"", 3),

                    Token(TokenType::SYMBOL, L"う", 3),
                    Token(TokenType::NEWL, L"", 4),
                    Token(TokenType::DEDENT, L"", 4),

                    Token(TokenType::SYMBOL, L"い", 4),
                    Token(TokenType::NEWL, L"", 5),
                    Token(TokenType::INDENT, L"", 5),

                    Token(TokenType::SYMBOL, L"う", 5),
                    Token(TokenType::NEWL, L"", 6),
                    Token(TokenType::DEDENT, L"", 6),
                    Token(TokenType::DEDENT, L"", 6),

                    Token(TokenType::SYMBOL, L"あ", 6),
                    Token(TokenType::NEWL, L"", 7),

                    Token(TokenType::DEDENT, L"", 7),
                    Token(TokenType::DEDENT, L"", 7),
                    Token(TokenType::END, L"", 7),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, infix) {
    auto stringInput = StringInputSource(
            L"ほげ＋ふが－ぴよ＊ほげ／ぴよ"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, L"ほげ", 1),
                    Token(TokenType::PLUS, L"＋", 1),
                    Token(TokenType::SYMBOL, L"ふが", 1),
                    Token(TokenType::MINUS, L"－", 1),
                    Token(TokenType::SYMBOL, L"ぴよ", 1),
                    Token(TokenType::STAR, L"＊", 1),
                    Token(TokenType::SYMBOL, L"ほげ", 1),
                    Token(TokenType::SLASH, L"／", 1),
                    Token(TokenType::SYMBOL, L"ぴよ", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, comparison) {
    auto stringInput = StringInputSource(
            L"あ＝＝い＜＝う＞＝え＜お＞か！＝き"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, L"あ", 1),
                    Token(TokenType::EQ, L"＝＝", 1),
                    Token(TokenType::SYMBOL, L"い", 1),
                    Token(TokenType::LEQ, L"＜＝", 1),
                    Token(TokenType::SYMBOL, L"う", 1),
                    Token(TokenType::GEQ, L"＞＝", 1),
                    Token(TokenType::SYMBOL, L"え", 1),
                    Token(TokenType::LT, L"＜", 1),
                    Token(TokenType::SYMBOL, L"お", 1),
                    Token(TokenType::GT, L"＞", 1),
                    Token(TokenType::SYMBOL, L"か", 1),
                    Token(TokenType::NEQ, L"！＝", 1),
                    Token(TokenType::SYMBOL, L"き", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, assert) {
    auto stringInput = StringInputSource(
            L"確認、０＝＝１"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::ASSERT, L"確認", 1),
                    Token(TokenType::COMMA, L"、", 1),
                    Token(TokenType::NUMBER, L"０", 1),
                    Token(TokenType::EQ, L"＝＝", 1),
                    Token(TokenType::NUMBER, L"１", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, braces) {
    // 隅付き格好
    auto stringInput = StringInputSource(
            L"鈴木さん【「名前」】"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, L"鈴木さん", 1),
                    Token(TokenType::LBRACE, L"【", 1),
                    Token(TokenType::STRING, L"名前", 1),
                    Token(TokenType::RBRACE, L"】", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, nonlocal_external) {
    // 隅付き格好
    auto stringInput = StringInputSource(
            L"外側、私の変数名"
    );

    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::EXTERN, L"外側", 1),
                    Token(TokenType::COMMA, L"、", 1),
                    Token(TokenType::SYMBOL, L"私の変数名", 1),
                    Token(TokenType::END, L"", 1),
            });
    EXPECT_EQ(allTokens, expected);
}
