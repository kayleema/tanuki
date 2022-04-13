#include "Context.h"
#include "CoreFunctions.h"
#include "Environment.h"
#include "Extension.h"
#include "Lexer/Tokenizer.h"
#include "Logger.h"
#include "Parser/Parser.h"
#include "TextInput/FileInputSource.h"
#include "TextInput/StringInputSource.h"
#include "helpText.h"
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

    // Parse command line args
    setlocale(LC_ALL, "");
    if (argc < 2) {
        log.logLn("使い方は間違い；引数はありません。");
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
            } else if (strcmp(argv[i], "lex") == 0) {
                print_lex = true;
            } else {
                log.logLn("使い方は間違い：「-d」フラグの後に意外な直");
                return 1;
            }
        }
        if (strcmp(argv[i], "-f") == 0) {
            i++;
            freq = atol(argv[i]);
            log.log("Freq set to ")->logLong(freq)->logEndl();
        }
        if (strcmp(argv[i], "-h") == 0) {
            log.log(TANUKI_HELP_MESSAGE);
            return 0;
        }
    }

    // Load file
    const char *sourceFilename = argv[argc - 1];
    log.log(L"実行始まります：")->log(sourceFilename)->logEndl();
    FileInputSource input(sourceFilename);

    // Tokenize
    auto tokenizer = TanukiTokenizer(&input);
    if (print_lex) {
        log.logLn("LEXER 結果:");
        Token current;
        while ((current = tokenizer.getToken()).type != TokenType::END) {
            log.logLn(current.toString());
        }
        log.logEndl();
        return 1;
    }

    // Parse
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    if (print_ast) {
        string treeString = tree->toString();
        log.logLn("ABSTRACT SYNTAX TREE:");
        log.logLn("---------------------");
        log.logLn(treeString);
        delete tree;
        return 1;
    }

    // Execute
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
