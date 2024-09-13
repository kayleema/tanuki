#ifndef PINPONPROJECT_MATCHERSYMBOL_H
#define PINPONPROJECT_MATCHERSYMBOL_H
#include "Lexer/Matcher.h"

class MatcherSymbol : public Matcher {
  public:
    MatcherResult match(TnkChar first, int currentLineNumber,
                        InputSource *input) override;

  private:
    static MatcherResult matchIdentifier(const string &tokenString,
                                         int currentLineNumber);
    static MatcherResult matchKanjiNumber(const string &tokenString,
                                            int currentLineNumber);
    static long parseKanjiNumber(string basicString);
};

#endif // PINPONPROJECT_MATCHERSYMBOL_H
