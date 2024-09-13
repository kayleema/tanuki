#ifndef PINPONPROJECT_MATCHERCOMMENT_H
#define PINPONPROJECT_MATCHERCOMMENT_H
#include "Lexer/Matcher.h"

class MatcherComment : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERCOMMENT_H
