#ifndef PINPONPROJECT_MATCHERSYMBOL_H
#define PINPONPROJECT_MATCHERSYMBOL_H
#include "Lexer/Matcher.h"

class MatcherSymbol : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERSYMBOL_H
