#include <iostream>

#include "gtest/gtest.h"

#include "Lexer/Tokenizer.h"
#include "TextInput/StringInputSource.h"
#include "TextInput/UnicodeConversion.h"

#include <random>
#include <climits>

TEST(tokenizer, simple_test) {
    auto stringInput = StringInputSource("関数、フィボナッチ（番号）");
    auto tokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = tokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::FUNC, "関数", 1),
                    Token(TokenType::COMMA, "、", 1),
                    Token(TokenType::SYMBOL, "フィボナッチ", 1),
                    Token(TokenType::LPAREN, "（", 1),
                    Token(TokenType::SYMBOL, "番号", 1),
                    Token(TokenType::RPAREN, "）", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, multiline) {
    auto stringInput = StringInputSource(
        "関数、フィボナッチ（番号）\n"
        "　返す、１２３"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
        {
            Token(TokenType::NEWL, "", 2),
            Token(TokenType::INDENT, "", 2),
            Token(TokenType::RETURN, "返す", 2),
            Token(TokenType::COMMA, "、", 2),
            Token(TokenType::NUMBER, "１２３", 2),
            Token(TokenType::END, "", 2),
        });
    EXPECT_EQ(vector<Token>(allTokens.begin() + 6, allTokens.end()), expected);
}

TEST(tokenizer, emptyline) {
    auto stringInput = StringInputSource("関数、フィボナッチ（番号）\n"
                                         "　返す、１２３\n"
                                         "\n"
                                         "フィボナッチ（４）");
    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    EXPECT_EQ(tokenListToTypeList(allTokens),
              "function, comma, symbol, lparen, symbol, rparen, newl, "
              "indent, return, comma, number, newl, "
              "dedent, newl, "
              "symbol, lparen, number, rparen, end");
}

TEST(tokenizer, parsesFloats) {
    auto stringInput = StringInputSource(
            "関数、フィボナッチ（番号）\n"
            "　返す、１２３。４"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::NEWL, "", 2),
                    Token(TokenType::INDENT, "", 2),
                    Token(TokenType::RETURN, "返す", 2),
                    Token(TokenType::COMMA, "、", 2),
                    Token(TokenType::NUMBER_FLOAT, "１２３。４", 2),
                    Token(TokenType::END, "", 2),
            });
    EXPECT_EQ(vector<Token>(allTokens.begin() + 6, allTokens.end()), expected);
}

TEST(tokenizer, functions) {
    auto stringInput = StringInputSource(
            "あ（い（）、う（））"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, "あ", 1),
                    Token(TokenType::LPAREN, "（", 1),
                    Token(TokenType::SYMBOL, "い", 1),
                    Token(TokenType::LPAREN, "（", 1),
                    Token(TokenType::RPAREN, "）", 1),
                    Token(TokenType::COMMA, "、", 1),
                    Token(TokenType::SYMBOL, "う", 1),
                    Token(TokenType::LPAREN, "（", 1),
                    Token(TokenType::RPAREN, "）", 1),
                    Token(TokenType::RPAREN, "）", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, comments) {
    auto stringInput = StringInputSource(
            "あ（）＃コメント\n"
            "あ（＃コメント"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, "あ", 1),
                    Token(TokenType::LPAREN, "（", 1),
                    Token(TokenType::RPAREN, "）", 1),
                    Token(TokenType::NEWL, "", 2),

                    Token(TokenType::SYMBOL, "あ", 2),
                    Token(TokenType::LPAREN, "（", 2),
                    Token(TokenType::END, "", 2),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, indentation) {
    auto stringInput = StringInputSource(
            "　　あ\n"
            "　　　い\n"
            "　　　　う\n"
            "　　　い\n"
            "　　　　う\n"
            "　　あ\n"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::INDENT, "", 1),
                    Token(TokenType::INDENT, "", 1),

                    Token(TokenType::SYMBOL, "あ", 1),
                    Token(TokenType::NEWL, "", 2),
                    Token(TokenType::INDENT, "", 2),

                    Token(TokenType::SYMBOL, "い", 2),
                    Token(TokenType::NEWL, "", 3),
                    Token(TokenType::INDENT, "", 3),

                    Token(TokenType::SYMBOL, "う", 3),
                    Token(TokenType::NEWL, "", 4),
                    Token(TokenType::DEDENT, "", 4),

                    Token(TokenType::SYMBOL, "い", 4),
                    Token(TokenType::NEWL, "", 5),
                    Token(TokenType::INDENT, "", 5),

                    Token(TokenType::SYMBOL, "う", 5),
                    Token(TokenType::NEWL, "", 6),
                    Token(TokenType::DEDENT, "", 6),
                    Token(TokenType::DEDENT, "", 6),

                    Token(TokenType::SYMBOL, "あ", 6),
                    Token(TokenType::NEWL, "", 7),

                    Token(TokenType::DEDENT, "", 7),
                    Token(TokenType::DEDENT, "", 7),
                    Token(TokenType::END, "", 7),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, infix) {
    auto stringInput = StringInputSource(
            "ほげ＋ふが－ぴよ＊ほげ／ぴよ"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, "ほげ", 1),
                    Token(TokenType::PLUS, "＋", 1),
                    Token(TokenType::SYMBOL, "ふが", 1),
                    Token(TokenType::MINUS, "－", 1),
                    Token(TokenType::SYMBOL, "ぴよ", 1),
                    Token(TokenType::STAR, "＊", 1),
                    Token(TokenType::SYMBOL, "ほげ", 1),
                    Token(TokenType::SLASH, "／", 1),
                    Token(TokenType::SYMBOL, "ぴよ", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, comparison) {
    auto stringInput = StringInputSource(
            "あ＝＝い＜＝う＞＝え＜お＞か！＝き"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, "あ", 1),
                    Token(TokenType::EQ, "＝＝", 1),
                    Token(TokenType::SYMBOL, "い", 1),
                    Token(TokenType::LEQ, "＜＝", 1),
                    Token(TokenType::SYMBOL, "う", 1),
                    Token(TokenType::GEQ, "＞＝", 1),
                    Token(TokenType::SYMBOL, "え", 1),
                    Token(TokenType::LT, "＜", 1),
                    Token(TokenType::SYMBOL, "お", 1),
                    Token(TokenType::GT, "＞", 1),
                    Token(TokenType::SYMBOL, "か", 1),
                    Token(TokenType::NEQ, "！＝", 1),
                    Token(TokenType::SYMBOL, "き", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, assert) {
    auto stringInput = StringInputSource(
            "確認、０＝＝１"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::ASSERT, "確認", 1),
                    Token(TokenType::COMMA, "、", 1),
                    Token(TokenType::NUMBER, "０", 1),
                    Token(TokenType::EQ, "＝＝", 1),
                    Token(TokenType::NUMBER, "１", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, braces) {
    // 隅付き格好
    auto stringInput = StringInputSource(
            "鈴木さん【「名前」】"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
            {
                    Token(TokenType::SYMBOL, "鈴木さん", 1),
                    Token(TokenType::LBRACE, "【", 1),
                    Token(TokenType::STRING, "名前", 1),
                    Token(TokenType::RBRACE, "】", 1),
                    Token(TokenType::END, "", 1),
            });
    EXPECT_EQ(allTokens, expected);
}

TEST(tokenizer, nonlocal_external) {
    // 隅付き格好
    auto stringInput = StringInputSource(
        "外側、私の変数名"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);
    auto allTokens = testTokenizer.getAllTokens();

    auto expected = vector<Token>(
        {
            Token(TokenType::EXTERN, "外側", 1),
            Token(TokenType::COMMA, "、", 1),
            Token(TokenType::SYMBOL, "私の変数名", 1),
            Token(TokenType::END, "", 1),
        });
    EXPECT_EQ(allTokens, expected);
}

static const char *const kanjiDigits[] = {"零", "一", "二", "三", "四",
                                          "五", "六", "七", "八", "九"};
static const char *const kanjiSuperPowers[] = {
    "京", "兆", "億", "万", "千", "百", "十",
};

string makeKanjiNumberString(long input) {
    if (input < 10) {
        return kanjiDigits[input];
    }

    long i = 10000000000000000;
    for (const char *power : kanjiSuperPowers) {
        if ((input / i) % i) {
            auto result = string("");
            if (input / i > 1) {
                result = makeKanjiNumberString(input / i);
            }
            result.append(power);
            if (input % i) {
                result.append(makeKanjiNumberString(input % i));
            }
            return result;
        }
        if ( i > 10000 ) {
            i /= 10000;
        } else {
            i /= 10;
        }
    }
    return "文字列化出来ませんでした。";
}

TEST(tokenizer, kanji_number_parsing_first_juuman) {
    for (long i = 0; i < 100000; i++) {
        auto stringSource = makeKanjiNumberString(i);
        auto cStringSource = stringSource.c_str();
        auto stringInput = StringInputSource(cStringSource);
        auto testTokenizer = TanukiTokenizer(&stringInput);
        auto token = testTokenizer.getToken();
        auto result = token.number;
        ASSERT_EQ(result, i);
        ASSERT_EQ(
            (token.type),
            (TokenType::NUMBER)
        );
    }
}

TEST(tokenizer, kanji_number_parsing_last_juuman) {
    for (long i = 0; i < 100000; i++) {
        long valueToTest = LONG_MAX - i;

        auto stringSource = makeKanjiNumberString(valueToTest);
        auto cStringSource = stringSource.c_str();
        auto stringInput = StringInputSource(cStringSource);
        auto testTokenizer = TanukiTokenizer(&stringInput);
        auto token = testTokenizer.getToken();
        auto result = token.number;
        ASSERT_EQ(result, valueToTest);
    }
}

TEST(tokenizer, kanji_number_parsing_fuzz_test) {
    cout << "max long is " << LONG_MAX << endl;
    std::default_random_engine generator;
    std::uniform_int_distribution<long> distribution(0,LONG_MAX);
    for (long i = 0; i < 100000; i++) {
        long valueToTest = distribution(generator);

        auto stringSource = makeKanjiNumberString(valueToTest);
        // cout << encodeUTF8(stringSource) << endl;
        auto cStringSource = stringSource.c_str();
        auto stringInput = StringInputSource(cStringSource);
        auto testTokenizer = TanukiTokenizer(&stringInput);
        auto token = testTokenizer.getToken();
        auto result = token.number;
        ASSERT_EQ(result, valueToTest);
    }
}
