
#include "Tokenizer.h"
#include "Lexer/Matchers/MatcherSingleCharacter.h"
#include "Lexer/Matchers/MatcherComparison.h"
#include "Lexer/Matchers/MatcherEOF.h"
#include "Lexer/Matchers/MatcherComment.h"
#include "Lexer/Matchers/MatcherNewline.h"
#include "Lexer/Matchers/MatcherNumberLiteral.h"
#include "Lexer/Matchers/MatcherString.h"
#include "Lexer/Matchers/MatcherSymbol.h"

TanukiTokenizer::TanukiTokenizer(InputSource *input) : InputSourceTokenizer(input) {
    matchers.push_back(new MatcherEOF());
    matchers.push_back(new MatcherComparison());
    matchers.push_back(new MatcherSingleCharacter());
    matchers.push_back(new MatcherComment());
    matchers.push_back(new MatcherNewline());
    matchers.push_back(new MatcherNumberLiteral());
    matchers.push_back(new MatcherString());
    matchers.push_back(new MatcherSymbol());
}


TanukiTokenizer::~TanukiTokenizer() {
    for (auto *matcher : matchers) {
        delete matcher;
    }
}
