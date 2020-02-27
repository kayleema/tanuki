#include <cstring>
#include <iostream>
#include "Tokenizer.h"
#include "Context.h"
#include "Parser.h"
#include "Environment.h"
#include "CoreFunctions.h"
#include <fcntl.h>
#include <locale.h>
#include "Extension.h"
#include "Logger.h"

#ifdef _WIN32
#include <io.h>
#else
#include <sys/uio.h>
#endif

using namespace std;

void evalPinponStarter(Environment *env);

int interactive(long freq);

int main(int argc, char **argv) {
    ConsoleLogger log;
    log.setup();

    setlocale(LC_ALL, "");
	if (argc < 2) {
	    log.logLn("使い方は間違い");
		return 1;
	}
	long freq = 100000;
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
			freq = atol(argv[i]);
            log.log("Freq set to ")
                    ->logLong(freq)
                    ->logEndl();
		}
		if (strcmp(argv[i], "-h") == 0) {
            log
                    .log("狸語プログラミング言語")->logEndl()
			        ->log("ーーーーーーーーーーーーーーー")->logEndl()
			        ->log("使い方：")->logEndl()
			        ->log("　./pinpon ファイル名.pin")->logEndl()
			        ->logEndl()
			        ->log("パラメーター：")->logEndl()
			        ->log("　-i：インタラクティブ・モード（REPL）")->logEndl()
			        ->log("　-d lex：lexerの結果を表示（ディバギング）")->logEndl()
			        ->log("　-d ast：parserの結果を表示（ディバギング）")->logEndl()
			        ->log("　-f 数字：evalの何回目の時にメモリを掃除（デフォルトは十万）")->logEndl()
			        ->log("　-h：このメッセジを表示")->logEndl();
			return 0;
		}
		if (strcmp(argv[i], "-i") == 0) {
            log.log("インタラクティブ・モード")->logEndl();
			return interactive(freq);
		}
	}

	const char *sourceFilename = argv[argc - 1];
    log.log(L"始まります")->log(sourceFilename)->logEndl();
    FileInputSource input(sourceFilename);
	auto t = InputSourceTokenizer(&input);

	if (print_lex) {
        log.logLn("LEXER 結果:");
		Token current;
		while ((current = t.getToken()).type != TokenType::END) {
            log.logLn(current.toString());
		}
		log.logEndl();
		return 1;
	}

	auto p = Parser(&t, &log);
	SyntaxNode *tree = p.run();

	if (print_ast) {
		string treeString = tree->toString();
		log.logLn("ABSTRACT SYNTAX TREE:");
        log.logLn("---------------------");
        log.logLn(treeString);
	}

	Context context;
	context.setFrequency(freq);
    FilesystemImpl filesystem;
    auto *env = new Environment(&context, &filesystem);
	env->bind(
		L"FILE",
		context.newStringValue(decodeUTF8(sourceFilename))
	);
	evalPinponStarter(env);
	env->eval(tree);

	delete tree;
	context.cleanup();
    return 0;
}

int interactive(long freq) {
    ConsoleLogger log;
	Context context;
	context.setFrequency(freq);
	auto *env = new Environment(&context);
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
        input += decodeUTF8(inputraw);
        input += L"\n";
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
		auto tokenizer = InputSourceTokenizer(&source);
		auto parser = Parser(&tokenizer, &log);
		SyntaxNode *tree = parser.run();
		if (!tree || tree->children.empty()) {
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
    ConsoleLogger log;
	auto source = StringInputSource(corePinponStarter);
	auto tokenizer = InputSourceTokenizer(&source);
	auto parser = Parser(&tokenizer, &log);
	SyntaxNode *tree = parser.run();
	env->eval(tree);
}
