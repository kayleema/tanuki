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
        "関数、フィボナッチ（番号）\n"
        "　返す、１２３"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "あ（い（）、う（））"
    );
    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "あ（）＃コメント\n"
            "あ（＃コメント"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "ほげ＋ふが－ぴよ＊ほげ／ぴよ"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "あ＝＝い＜＝う＞＝え＜お＞か！＝き"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "確認、０＝＝１"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

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
            "鈴木さん【「名前」】"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);

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
        "外側、私の変数名"
    );

    auto testTokenizer = TanukiTokenizer(&stringInput);
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
        ASSERT_EQ(token.type, TokenType::NUMBER);
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
