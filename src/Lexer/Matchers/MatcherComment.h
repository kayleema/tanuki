#ifndef PINPONPROJECT_MATCHERCOMMENT_H
#define PINPONPROJECT_MATCHERCOMMENT_H
#include "Lexer/Matcher.h"

class MatcherComment : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERCOMMENT_H
