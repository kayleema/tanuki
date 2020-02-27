#include "gtest/gtest.h"

#include "Tokenizer.h"
#include "Parser.h"
#include "fakeit.hpp"

TEST(parsing, functions) {

    auto stringInput = StringInputSource(
            L"あ（い（）、４５６、う（１２３））（え）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”あ”、1行目\n"
            u8" CALL_TAIL\n"
            u8"  ARGS\n"
            u8"   CALL\n"
            u8"    TERMINAL symbol：”い”、1行目\n"
            u8"    CALL_TAIL\n"
            u8"     ARGS\n"
            u8"   TERMINAL number：”４５６（456）”、1行目\n"
            u8"   CALL\n"
            u8"    TERMINAL symbol：”う”、1行目\n"
            u8"    CALL_TAIL\n"
            u8"     ARGS\n"
            u8"      TERMINAL number：”１２３（123）”、1行目\n"
            u8"  CALL_TAIL\n"
            u8"   ARGS\n"
            u8"    TERMINAL symbol：”え”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, importing_file) {
    auto stringInput = StringInputSource(
            L"導入、フォルダー・フォルダー２・ファイル名"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"IMPORT\n"
            u8" TERMINAL symbol：”フォルダー”、1行目\n"
            u8" TERMINAL symbol：”フォルダー２”、1行目\n"
            u8" TERMINAL symbol：”ファイル名”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, function_with_kwargs) {
    auto stringInput = StringInputSource(
            L"関数名前（引数一、引数二、キー１：バリュー１、キー２：バリュー２）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”関数名前”、1行目\n"
            u8" CALL_TAIL\n"
            u8"  ARGS\n"
            u8"   TERMINAL symbol：”引数一”、1行目\n"
            u8"   TERMINAL symbol：”引数二”、1行目\n"
            u8"   KWARG\n"
            u8"    TERMINAL symbol：”キー１”、1行目\n"
            u8"    TERMINAL symbol：”バリュー１”、1行目\n"
            u8"   KWARG\n"
            u8"    TERMINAL symbol：”キー２”、1行目\n"
            u8"    TERMINAL symbol：”バリュー２”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, user_function) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"FUNC\n"
            u8" TERMINAL symbol：”ほげ”、1行目\n"
            u8" PARAMS\n"
            u8"  TERMINAL symbol：”引数”、1行目\n"
            u8"  VARKWPARAM\n"
            u8"   TERMINAL symbol：”辞書引数”、1行目\n"
            u8" TEXT\n"
            u8"  RETURN\n"
            u8"   TERMINAL number：”１（1）”、2行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, user_function_varparam) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();

    SyntaxNode expectedTree(NodeType::FUNC, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"ほげ", 1)),
            new SyntaxNode(NodeType::PARAMS, {
                    new SyntaxNode(Token(TokenType::SYMBOL, L"引数", 1)),
                    new SyntaxNode(NodeType::VARPARAM, {
                            new SyntaxNode(Token(TokenType::SYMBOL, L"配列引数", 1))
                    }),
                    new SyntaxNode(NodeType::VARKWPARAM, {
                            new SyntaxNode(Token(TokenType::SYMBOL, L"辞書引数", 1))
                    })
            }),
            new SyntaxNode(NodeType::TEXT, {
                    new SyntaxNode(NodeType::RETURN, {
                            new SyntaxNode(Token(TokenType::NUMBER, L"１", 2))
                    })
            })
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}

TEST(parsing, user_function_defaultparam) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、あ：１＋２、＊配列引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();

    SyntaxNode expectedTree(NodeType::FUNC, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"ほげ", 1)),
            new SyntaxNode(NodeType::PARAMS, {
                    new SyntaxNode(Token(TokenType::SYMBOL, L"引数", 1)),
                    new SyntaxNode(NodeType::DEFAULTPARAM, {
                            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
                            new SyntaxNode(NodeType::ADD, {
                                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
                            }),
                    }),
                    new SyntaxNode(NodeType::VARPARAM, {
                            new SyntaxNode(Token(TokenType::SYMBOL, L"配列引数", 1))
                    }),
                    new SyntaxNode(NodeType::VARKWPARAM, {
                            new SyntaxNode(Token(TokenType::SYMBOL, L"辞書引数", 1))
                    })
            }),
            new SyntaxNode(NodeType::TEXT, {
                    new SyntaxNode(NodeType::RETURN, {
                            new SyntaxNode(Token(TokenType::NUMBER, L"１", 2))
                    })
            })
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}

TEST(parsing, dot_lookup) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”ほげ”、1行目\n"
            u8" GET\n"
            u8"  TERMINAL symbol：”何か”、1行目\n"
            u8"  GET\n"
            u8"   TERMINAL symbol：”ほか”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_assign) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか＝あ・い・う"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”ほげ”、1行目\n"
            u8" GET\n"
            u8"  TERMINAL symbol：”何か”、1行目\n"
            u8"  SET\n"
            u8"   TERMINAL symbol：”ほか”、1行目\n"
            u8"   CALL\n"
            u8"    TERMINAL symbol：”あ”、1行目\n"
            u8"    GET\n"
            u8"     TERMINAL symbol：”い”、1行目\n"
            u8"     GET\n"
            u8"      TERMINAL symbol：”う”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_call) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか（）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”ほげ”、1行目\n"
            u8" GET\n"
            u8"  TERMINAL symbol：”何か”、1行目\n"
            u8"  GET\n"
            u8"   TERMINAL symbol：”ほか”、1行目\n"
            u8"   CALL_TAIL\n"
            u8"    ARGS\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_call_and_lookup) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか（）・あ・い・う"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”ほげ”、1行目\n"
            u8" GET\n"
            u8"  TERMINAL symbol：”何か”、1行目\n"
            u8"  GET\n"
            u8"   TERMINAL symbol：”ほか”、1行目\n"
            u8"   CALL_TAIL\n"
            u8"    ARGS\n"
            u8"    GET\n"
            u8"     TERMINAL symbol：”あ”、1行目\n"
            u8"     GET\n"
            u8"      TERMINAL symbol：”い”、1行目\n"
            u8"      GET\n"
            u8"       TERMINAL symbol：”う”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, simple) {
    auto stringInput = StringInputSource(
            L"１－２＋３"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();

    auto expectedTree = SyntaxNode(NodeType::ADD, {
            new SyntaxNode(NodeType::SUB, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            }),
            new SyntaxNode(Token(TokenType::NUMBER, L"３", 1))
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}

TEST(parsing_infix, complex) {
    auto stringInput = StringInputSource(
            L"１－２＋あ（３）＋い"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"ADD\n"
            u8" ADD\n"
            u8"  SUB\n"
            u8"   TERMINAL number：”１（1）”、1行目\n"
            u8"   TERMINAL number：”２（2）”、1行目\n"
            u8"  CALL\n"
            u8"   TERMINAL symbol：”あ”、1行目\n"
            u8"   CALL_TAIL\n"
            u8"    ARGS\n"
            u8"     TERMINAL number：”３（3）”、1行目\n"
            u8" TERMINAL symbol：”い”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, within) {
    auto stringInput = StringInputSource(
            L"表示（１－２＋３）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            u8"CALL\n"
            u8" TERMINAL symbol：”表示”、1行目\n"
            u8" CALL_TAIL\n"
            u8"  ARGS\n"
            u8"   ADD\n"
            u8"    SUB\n"
            u8"     TERMINAL number：”１（1）”、1行目\n"
            u8"     TERMINAL number：”２（2）”、1行目\n"
            u8"    TERMINAL number：”３（3）”、1行目\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, comparison) {
    auto stringInput = StringInputSource(
            L"表示（１＝＝２）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::CALL, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"表示", 1)),
            new SyntaxNode(NodeType::CALL_TAIL, {
                    new SyntaxNode(NodeType::ARGS, {
                            new SyntaxNode(NodeType::EQUAL, {
                                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
                            })
                    })
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, comparisonNotEqual) {
    auto stringInput = StringInputSource(
            L"あ＝１！＝２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSIGN, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
            new SyntaxNode(NodeType::NEQ, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, comparisonLessThan) {
    auto stringInput = StringInputSource(
            L"あ＝１＜２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSIGN, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
            new SyntaxNode(NodeType::LT, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, comparisonGreaterThan) {
    auto stringInput = StringInputSource(
            L"あ＝１＞２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSIGN, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
            new SyntaxNode(NodeType::GT, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, comparisonGreaterThanEqual) {
    auto stringInput = StringInputSource(
            L"あ＝１＞＝２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSIGN, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
            new SyntaxNode(NodeType::GTE, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, comparisonLessThanEqual) {
    auto stringInput = StringInputSource(
            L"あ＝１＜＝２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSIGN, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"あ", 1)),
            new SyntaxNode(NodeType::LTE, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_infix, assert) {
    auto stringInput = StringInputSource(
            L"確認、１＝＝２"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expected(NodeType::ASSERT, {
            new SyntaxNode(NodeType::EQUAL, {
                    new SyntaxNode(Token(TokenType::NUMBER, L"１", 1)),
                    new SyntaxNode(Token(TokenType::NUMBER, L"２", 1))
            })
    });
    EXPECT_EQ(expected, *tree->children[0]);
}

TEST(parsing_subscript, subscript) {
    auto stringInput = StringInputSource(
            L"辞書（名前：「すずき」）【「名前」】"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expectedTree(NodeType::CALL, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"辞書", 1)),
            new SyntaxNode(NodeType::CALL_TAIL, {
                    new SyntaxNode(NodeType::ARGS, {
                            new SyntaxNode(NodeType::KWARG, {
                                    new SyntaxNode(Token(TokenType::SYMBOL, L"名前", 1)),
                                    new SyntaxNode(Token(TokenType::STRING, L"すずき", 1))
                            }),
                    }),
                    new SyntaxNode(NodeType::SUBSCRIPT, {
                            new SyntaxNode(Token(TokenType::STRING, L"名前", 1))
                    })
            })
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}


TEST(parsing_subscript, subscript_complex) {
    auto stringInput = StringInputSource(
            L"辞書（名前：「すずき」）【「名前」】・長さ"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expectedTree(NodeType::CALL, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"辞書", 1)),
            new SyntaxNode(NodeType::CALL_TAIL, {
                    new SyntaxNode(NodeType::ARGS, {
                            new SyntaxNode(NodeType::KWARG, {
                                    new SyntaxNode(Token(TokenType::SYMBOL, L"名前", 1)),
                                    new SyntaxNode(Token(TokenType::STRING, L"すずき", 1))
                            }),
                    }),
                    new SyntaxNode(NodeType::SUBSCRIPT, {
                            new SyntaxNode(Token(TokenType::STRING, L"名前", 1)),
                            new SyntaxNode(NodeType::GET, {
                                    new SyntaxNode(Token(TokenType::SYMBOL, L"長さ", 1))
                            })
                    })
            })
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}

TEST(parsing_subscript, subscript_set) {
    auto stringInput = StringInputSource(
            L"辞書（名前：「すずき」）【「名前」】＝「田中」"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer, nullptr);

    SyntaxNode *tree = parser.run();

    SyntaxNode expectedTree(NodeType::CALL, {
            new SyntaxNode(Token(TokenType::SYMBOL, L"辞書", 1)),
            new SyntaxNode(NodeType::CALL_TAIL, {
                    new SyntaxNode(NodeType::ARGS, {
                            new SyntaxNode(NodeType::KWARG, {
                                    new SyntaxNode(Token(TokenType::SYMBOL, L"名前", 1)),
                                    new SyntaxNode(Token(TokenType::STRING, L"すずき", 1))
                            }),
                    }),
                    new SyntaxNode(NodeType::SUBSCRIPT_SET, {
                            new SyntaxNode(Token(TokenType::STRING, L"名前", 1)),
                            new SyntaxNode(Token(TokenType::STRING, L"田中", 1))
                    })
            })
    });
    EXPECT_EQ(expectedTree, *tree->children[0]);
}

using namespace fakeit;

TEST(parsing, parse_error) {
    auto stringInput = StringInputSource(
            L"導入＋１"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    fakeit::Mock<PinponLogger> logger;
    When(OverloadedMethod(logger, log, PinponLogger * (string))).AlwaysReturn(&logger.get());
    auto parser = Parser(&testTokenizer, &logger.get());
    SyntaxNode *tree = parser.run();

    EXPECT_EQ(tree->type, NodeType::PARSE_ERROR);
    Verify(
            OverloadedMethod(logger, log, PinponLogger * (string))
            .Using("意外なトークン：plus：”＋”、1行目\n")
    );
}
