#ifndef PINPONPROJECT_MATCHEREOF_H
#define PINPONPROJECT_MATCHEREOF_H
#include "Lexer/Matcher.h"

class MatcherEOF : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHEREOF_H
