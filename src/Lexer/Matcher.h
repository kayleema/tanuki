#ifndef MATCHER_H
#define MATCHER_H

#include "TextInput/InputSource.h"
#include "Token.h"
#include <optional>
#include <utility>

class MatcherResult {
  public:
    bool matched;
    int incrementLineNumber;
    Token result;

    explicit MatcherResult(Token result, int incrementLine = 0)
        : matched(true), incrementLineNumber(incrementLine),
          result(std::move(result)) {}
    MatcherResult() : matched(false), incrementLineNumber(0), result() {}
};

class Matcher {
  public:
    virtual MatcherResult match(TnkChar first, int currentLineNumber,
                                InputSource *input) = 0;

    virtual ~Matcher() = default;
};

#endif // MATCHER_H
