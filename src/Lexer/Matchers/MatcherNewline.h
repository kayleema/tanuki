#ifndef PINPONPROJECT_MATCHERNEWLINE_H
#define PINPONPROJECT_MATCHERNEWLINE_H
#include "Lexer/Matcher.h"

class MatcherNewline : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;
};

#endif // PINPONPROJECT_MATCHERNEWLINE_H
