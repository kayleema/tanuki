#ifndef MATCHERCOMPARISON_H
#define MATCHERCOMPARISON_H

#include "Lexer/Matcher.h"

class MatcherComparison : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // MATCHERCOMPARISON_H
