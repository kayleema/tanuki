#include <iostream>
#include "Tokenizer.h"
#include "Parser.h"
#include "Evaluate.h"
#include <fcntl.h>
#include <sys/uio.h>
#include <locale.h>

using namespace std;

int main(int argc, char **argv) 
{
	setlocale(LC_ALL, "");
    std::cout << "始まります" << std::endl;
	if (argc != 2) {
		return 1;
	}
	auto input = FileInputSource(argv[1]);
	auto t = FileTokenizer(&input);

	// cout << "=============" << endl;
	// cout << "LEXER OUTPUT:" << endl;
	// Token current;
	// while ((current = t.getToken()).type != TokenType::END) {
	// 	cout << (current.toString()) << endl;
	// }
	// cout << endl;

	auto p = Parser(&t);

	SyntaxNode *tree = p.run();
	string treeString = tree->toString();
	cout << "ABSTRACT SYNTAX TREE:" << endl;
	cout << "---------------------" << endl;
	cout << treeString << endl;

	Environment env;
	Value *v = env.eval(tree);
	cout << "結果：" << v->toString() << endl;

	// auto stringInput = StringInputSource(L"変数、フィボナッチ（番号）");
	// auto testTokenizer = FileTokenizer(&stringInput);
	// Token token1 = testTokenizer.getToken();
	// auto allTokens = testTokenizer.getAllTokens();
	// cout << token1.toString() << allTokens.size() << endl;
    return 0;
}
