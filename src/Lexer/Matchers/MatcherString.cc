#include "MatcherString.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"
#include <iostream>

MatcherResult MatcherString::match(wchar_t first, int currentLineNumber,
                                   InputSource *input) {
    if (first == lsquare) {
        wstring resultString = wstring(L"");
        wchar_t nextChar;
        int lineCount = 0;
        while ((nextChar = input->getChar()) != rsquare) {
            resultString.push_back(nextChar);
            if (nextChar == newline) {
                lineCount++;
            }
            if (input->eof()) {
                std::cout
                    << u8"エラー：文字列を読みながら、ファイルの終わり（ＥＯＦ"
                       u8"）"
                    << endl;
                return MatcherResult(
                    Token(TokenType::END, L"", currentLineNumber));
            }
        }
        return MatcherResult(Token(TokenType::STRING, wstring(resultString),
                                   currentLineNumber + lineCount),
                             lineCount);
    }
    return {};
}
