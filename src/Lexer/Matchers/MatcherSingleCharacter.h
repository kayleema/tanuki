#ifndef MATCHERSINGLECHARACTER_H
#define MATCHERSINGLECHARACTER_H

#include "Lexer/Matcher.h"

class MatcherSingleCharacter : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // MATCHERSINGLECHARACTER_H
