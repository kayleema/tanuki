#include "Context.h"
#include "CoreFunctions.h"
#include "Environment.h"
#include "Extension.h"
#include "Lexer/Tokenizer.h"
#include "Logger.h"
#include "Parser.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <locale.h>

#ifdef _WIN32
#include <io.h>
#else
#include <sys/uio.h>
#endif

using namespace std;

void evalTanukiStarter(Environment *env);

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
            log.log("Freq set to ")->logLong(freq)->logEndl();
        }
        if (strcmp(argv[i], "-h") == 0) {
            log.log("狸語プログラミング言語")
                ->logEndl()
                ->log("ーーーーーーーーーーーーーーー")
                ->logEndl()
                ->log("使い方：")
                ->logEndl()
                ->log("　./tanuki ファイル名.tnk")
                ->logEndl()
                ->logEndl()
                ->log("パラメーター：")
                ->logEndl()
                ->log("　-d lex：lexerの結果を表示（ディバギング）")
                ->logEndl()
                ->log("　-d ast：parserの結果を表示（ディバギング）")
                ->logEndl()
                ->log(
                    "　-f "
                    "数字：evalの何回目の時にメモリを掃除（デフォルトは十万）")
                ->logEndl()
                ->log("　-h：このメッセジを表示")
                ->logEndl();
            return 0;
        }
    }

    const char *sourceFilename = argv[argc - 1];
    log.log(L"始まります")->log(sourceFilename)->logEndl();
    FileInputSource input(sourceFilename);
    auto t = TanukiTokenizer(&input);

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
        delete tree;
        return 1;
    }

    Context context;
    context.setFrequency(freq);
    FilesystemImpl filesystem;
    auto *env = new Environment(&context, &filesystem);
    env->bind(L"FILE", context.newStringValue(decodeUTF8(sourceFilename)));
    evalTanukiStarter(env);
    env->eval(tree);

    delete tree;
    context.cleanup();
    return 0;
}

void evalTanukiStarter(Environment *env) {
    ConsoleLogger log;
    auto source = StringInputSource(coreTanukiStarter);
    auto tokenizer = TanukiTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    env->eval(tree);
}
