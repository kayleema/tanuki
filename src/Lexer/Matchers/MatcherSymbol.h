#ifndef PINPONPROJECT_MATCHERSYMBOL_H
#define PINPONPROJECT_MATCHERSYMBOL_H
#include "Lexer/Matcher.h"

class MatcherSymbol : public Matcher {
  public:
    MatcherResult match(wchar_t first, int currentLineNumber,
                        InputSource *input) override;

  private:
    static MatcherResult matchIdentifier(const wstring &tokenString,
                                         int currentLineNumber);
    static MatcherResult matchKanjiNumber(const wstring &tokenString,
                                            int currentLineNumber);
    static long parseKanjiNumber(wstring basicString);
};

#endif // PINPONPROJECT_MATCHERSYMBOL_H
