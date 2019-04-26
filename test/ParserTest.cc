#include "gtest/gtest.h"

#include "Tokenizer.h"
#include "Parser.h"

TEST(parser, functions)
{
    auto stringInput = StringInputSource(
            L"あ（い（）、４５６、う（１２３））（え）"
    );
    auto testTokenizer = FileTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”あ”、1列\n"
            " CALL_TAIL\n"
            "  ARGS\n"
            "   CALL\n"
            "    TERMINAL symbol：”い”、1列\n"
            "    CALL_TAIL\n"
            "     ARGS\n"
            "   TERMINAL number：”４５６（456）”、1列\n"
            "   CALL\n"
            "    TERMINAL symbol：”う”、1列\n"
            "    CALL_TAIL\n"
            "     ARGS\n"
            "      TERMINAL number：”１２３（123）”、1列\n"
            "  CALL_TAIL\n"
            "   ARGS\n"
            "    TERMINAL symbol：”え”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parser, function_with_kwargs)
{
    auto stringInput = StringInputSource(
            L"関数名前（引数一、引数二、キー１：バリュー１、キー２：バリュー２）"
    );
    auto testTokenizer = FileTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”関数名前”、1列\n"
            " CALL_TAIL\n"
            "  ARGS\n"
            "   TERMINAL symbol：”引数一”、1列\n"
            "   TERMINAL symbol：”引数二”、1列\n"
            "   KWARG\n"
            "    TERMINAL symbol：”キー１”、1列\n"
            "    TERMINAL symbol：”バリュー１”、1列\n"
            "   KWARG\n"
            "    TERMINAL symbol：”キー２”、1列\n"
            "    TERMINAL symbol：”バリュー２”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parser, user_function)
{
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = FileTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
        "FUNC\n"
        " TERMINAL symbol：”ほげ”、1列\n"
        " PARAMS\n"
        "  TERMINAL symbol：”引数”、1列\n"
        "  VARKWPARAM\n"
        "   TERMINAL symbol：”辞書引数”、1列\n"
        " TEXT\n"
        "  RETURN\n"
        "   TERMINAL number：”１（1）”、2列\n"
    );
    EXPECT_EQ(expected, treeString);
}
