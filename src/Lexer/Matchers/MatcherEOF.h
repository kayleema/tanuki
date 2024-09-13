#ifndef PINPONPROJECT_MATCHEREOF_H
#define PINPONPROJECT_MATCHEREOF_H
#include "Lexer/Matcher.h"

class MatcherEOF : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHEREOF_H
