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
	if (argc < 2) {
		cout << "Incorrect Usage" << endl;
		return 1;
	}
	bool print_ast = false;
	bool print_lex = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-d") == 0) {
			i++;
			if (strcmp(argv[i], "ast") == 0) {
				print_ast = true;
			}
			if (strcmp(argv[i], "lex") == 0) {
				print_lex = true;
			}
		}
		if (strcmp(argv[i], "-h") == 0) {
			cout << "PinPon プログラミング言語" << endl;
			cout << "使い方：" << endl;
			cout << "./pinpon ソースコードのファイル名.pin" << endl << endl;
			return 0;
		}
	}

    std::cout << "始まります" << std::endl;
	auto input = FileInputSource(argv[argc - 1]);
	auto t = FileTokenizer(&input);

	if (print_lex) {
		cout << "LEXER OUTPUT:" << endl;
		Token current;
		while ((current = t.getToken()).type != TokenType::END) {
			cout << (current.toString()) << endl;
		}
		cout << endl;
		return 1;
	}

	auto p = Parser(&t);
	SyntaxNode *tree = p.run();

	if (print_ast) {
		string treeString = tree->toString();
		cout << "ABSTRACT SYNTAX TREE:" << endl;
		cout << "---------------------" << endl;
		cout << treeString << endl;
	}

	Context context;
	Environment *env = new Environment(&context);
	env->eval(tree);

	delete tree;
	context.cleanup();
    return 0;
}
