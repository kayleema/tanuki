#include "starterEvaluator.h"
#include "CoreFunctions.h"

void evalPinponStarter(Environment *env) {
    ConsoleLogger log;
    auto source = StringInputSource(corePinponStarter);
    auto tokenizer = TanukiTokenizer(&source);
    auto parser = Parser(&tokenizer, &log);
    SyntaxNode *tree = parser.run();
    env->eval(tree);
}
