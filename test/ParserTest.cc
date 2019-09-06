#include "gtest/gtest.h"

#include "Tokenizer.h"
#include "Parser.h"

TEST(parsing, functions) {

    auto stringInput = StringInputSource(
            L"あ（い（）、４５６、う（１２３））（え）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
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

TEST(parsing, importing_file) {
    auto stringInput = StringInputSource(
            L"導入、フォルダー・フォルダー２・ファイル名"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "IMPORT\n"
            " TERMINAL symbol：”フォルダー”、1列\n"
            " TERMINAL symbol：”フォルダー２”、1列\n"
            " TERMINAL symbol：”ファイル名”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, function_with_kwargs) {
    auto stringInput = StringInputSource(
            L"関数名前（引数一、引数二、キー１：バリュー１、キー２：バリュー２）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
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

TEST(parsing, user_function) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
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

TEST(parsing, user_function_varparam) {
    auto stringInput = StringInputSource(
            L"関数、ほげ（引数、＊配列引数、＊＊辞書引数）\n"
            L"　返す、１\n"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "FUNC\n"
            " TERMINAL symbol：”ほげ”、1列\n"
            " PARAMS\n"
            "  TERMINAL symbol：”引数”、1列\n"
            "  VARPARAM\n"
            "   TERMINAL symbol：”配列引数”、1列\n"
            "  VARKWPARAM\n"
            "   TERMINAL symbol：”辞書引数”、1列\n"
            " TEXT\n"
            "  RETURN\n"
            "   TERMINAL number：”１（1）”、2列\n"
    );
    EXPECT_EQ(expected, treeString);
}


TEST(parsing, dot_lookup) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”ほげ”、1列\n"
            " GET\n"
            "  TERMINAL symbol：”何か”、1列\n"
            "  GET\n"
            "   TERMINAL symbol：”ほか”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_assign) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか＝あ・い・う"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”ほげ”、1列\n"
            " GET\n"
            "  TERMINAL symbol：”何か”、1列\n"
            "  SET\n"
            "   TERMINAL symbol：”ほか”、1列\n"
            "   CALL\n"
            "    TERMINAL symbol：”あ”、1列\n"
            "    GET\n"
            "     TERMINAL symbol：”い”、1列\n"
            "     GET\n"
            "      TERMINAL symbol：”う”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_call) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか（）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”ほげ”、1列\n"
            " GET\n"
            "  TERMINAL symbol：”何か”、1列\n"
            "  GET\n"
            "   TERMINAL symbol：”ほか”、1列\n"
            "   CALL_TAIL\n"
            "    ARGS\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing, dot_lookup_and_call_and_lookup) {
    auto stringInput = StringInputSource(
            L"ほげ・何か・ほか（）・あ・い・う"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”ほげ”、1列\n"
            " GET\n"
            "  TERMINAL symbol：”何か”、1列\n"
            "  GET\n"
            "   TERMINAL symbol：”ほか”、1列\n"
            "   CALL_TAIL\n"
            "    ARGS\n"
            "    GET\n"
            "     TERMINAL symbol：”あ”、1列\n"
            "     GET\n"
            "      TERMINAL symbol：”い”、1列\n"
            "      GET\n"
            "       TERMINAL symbol：”う”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, simple) {
    auto stringInput = StringInputSource(
            L"１－２＋３"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "ADD\n"
            " SUB\n"
            "  TERMINAL number：”１（1）”、1列\n"
            "  TERMINAL number：”２（2）”、1列\n"
            " TERMINAL number：”３（3）”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, complex) {
    auto stringInput = StringInputSource(
            L"１－２＋あ（３）＋い"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "ADD\n"
            " ADD\n"
            "  SUB\n"
            "   TERMINAL number：”１（1）”、1列\n"
            "   TERMINAL number：”２（2）”、1列\n"
            "  CALL\n"
            "   TERMINAL symbol：”あ”、1列\n"
            "   CALL_TAIL\n"
            "    ARGS\n"
            "     TERMINAL number：”３（3）”、1列\n"
            " TERMINAL symbol：”い”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}

TEST(parsing_infix, within) {
    auto stringInput = StringInputSource(
            L"表示（１－２＋３）"
    );
    auto testTokenizer = InputSourceTokenizer(&stringInput);
    auto parser = Parser(&testTokenizer);
    SyntaxNode *tree = parser.run();
    string treeString = tree->children[0]->toString();
    string expected = (
            "CALL\n"
            " TERMINAL symbol：”表示”、1列\n"
            " CALL_TAIL\n"
            "  ARGS\n"
            "   ADD\n"
            "    SUB\n"
            "     TERMINAL number：”１（1）”、1列\n"
            "     TERMINAL number：”２（2）”、1列\n"
            "    TERMINAL number：”３（3）”、1列\n"
    );
    EXPECT_EQ(expected, treeString);
}
