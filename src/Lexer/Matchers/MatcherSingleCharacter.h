#ifndef MATCHERSINGLECHARACTER_H
#define MATCHERSINGLECHARACTER_H

#include "Lexer/Matcher.h"

class MatcherSingleCharacter : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // MATCHERSINGLECHARACTER_H
