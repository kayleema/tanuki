#ifndef PINPONPROJECT_MATCHERNEWLINE_H
#define PINPONPROJECT_MATCHERNEWLINE_H
#include "Lexer/Matcher.h"

class MatcherNewline : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERNEWLINE_H
