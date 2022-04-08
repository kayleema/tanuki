#ifndef PINPONPROJECT_MATCHERSTRING_H
#define PINPONPROJECT_MATCHERSTRING_H
#include "Lexer/Matcher.h"

class MatcherString : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERSTRING_H
