#ifndef PINPONPROJECT_MATCHERNUMBERLITERAL_H
#define PINPONPROJECT_MATCHERNUMBERLITERAL_H
#include "Lexer/Matcher.h"

class MatcherNumberLiteral : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERNUMBERLITERAL_H
