#include "starterEvaluator.h"
#include "CoreFunctions.h"

void evalTanukiStarter(Environment *env) {
    ConsoleLogger log;
    auto source = StringInputSource(coreTanukiStarter);
    auto tokenizer = TanukiTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    env->eval(tree);
}
