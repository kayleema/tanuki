#include <iostream>
#include "Tokenizer.h"
#include "Context.h"
#include "Parser.h"
#include "Evaluate.h"
#include "CoreFunctions.h"
#include <fcntl.h>
#include <sys/uio.h>
#include <locale.h>

using namespace std;

void evalPinponStarter(Environment *env);

int interractive(int freq);

int main(int argc, char **argv) 
{
	setlocale(LC_ALL, "");
	if (argc < 2) {
		cout << "使い方は間違い" << endl;
		return 1;
	}
	int freq = 10000;
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
		if (strcmp(argv[i], "-f") == 0) {
			i++;
			freq = atoi(argv[i]);
			cout << "Freq set to " << freq << endl;
		}
		if (strcmp(argv[i], "-h") == 0) {
			cout << "ＰｉｎＰｏｎプログラミング言語" << endl;
			cout << "ーーーーーーーーーーーーーーー" << endl;
			cout << "使い方：" << endl;
			cout << "　./pinpon ファイル名.pin" << endl;
			cout << endl;
			cout << "パラメーター：" << endl;
			cout << "　-i：インタラクティブ・モード（REPL）" << endl;
			cout << "　-d lex：lexerの結果を表示（ディバギング）" << endl;
			cout << "　-d ast：parserの結果を表示（ディバギング）" << endl;
			cout << "　-f 数字：evalの何回目の時にメモリを掃除（デフォルトは１万）" << endl;
			cout << "　-h：このメッセジを表示" << endl << endl;
			return 0;
		}
		if (strcmp(argv[i], "-i") == 0) {
			cout << "インタラクティブ・モード" << endl;
			return interractive(freq);
		}
	}

    std::cout << "始まります" << std::endl;
	auto input = FileInputSource(argv[argc - 1]);
	auto t = FileTokenizer(&input);

	if (print_lex) {
		cout << "LEXER 結果:" << endl;
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
	context.setFrequency(freq);
	Environment *env = new Environment(&context);
	evalPinponStarter(env);
	env->eval(tree);

	delete tree;
	context.cleanup();
    return 0;
}

wstring decodeUTF8(const string &in) {
	try
    {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
	    return conv1.from_bytes(in);
    }
    catch (const std::range_error & exception)
    {
        return wstring(L"XXX");
    }
}

int interractive(int freq) {
	Context context;
	context.setFrequency(freq);
	Environment *env = new Environment(&context);
	evalPinponStarter(env);
	string inputraw;
	wstring input;
	bool continuation = false;
	while (true) {
		if(continuation) {
			cout << "｜：";
		} else {
			cout << "＞：";
		}
		getline (cin, inputraw);
		input = input + decodeUTF8(inputraw) + L"\n";
		if (inputraw.find("関数") == 0 ||
			inputraw.find("もし") == 0 ||
			(continuation && (inputraw.length() != 0))) {
			continuation = true;
			continue;
		} else if (continuation) {
			continuation = false;
		}

		if (inputraw == "終わり") {
			return 0;
		}

		auto source = StringInputSource(input.c_str());
		auto tokenizer = FileTokenizer(&source);
		auto parser = Parser(&tokenizer);
		SyntaxNode *tree = parser.run();
		if (!tree || tree->children.size() == 0) {
			cout << "parser fail" << endl;
			input = L"";
			continue;
		}
		Value *result = env->eval(tree->children[0]);
		cout << result->toString() << endl;

		input = L"";
	}
}

void evalPinponStarter(Environment *env) {
	auto source = StringInputSource(corePinponStarter);
	auto tokenizer = FileTokenizer(&source);
	auto parser = Parser(&tokenizer);
	SyntaxNode *tree = parser.run();
	env->eval(tree);
}
