#include "MatcherString.h"
#include "Lexer/LexerConstants.h"
#include "Lexer/Matcher.h"
#include <iostream>
#include "TextInput/UnicodeConversion.h"

MatcherResult MatcherString::match(TnkChar first, int currentLineNumber,
                                   InputSource *input) {
    if (first == lsquare) {
        string resultString = string("");
        TnkChar nextChar;
        int lineCount = 0;
        while ((nextChar = input->getChar()) != rsquare) {
            resultString.append(tnkCharToString(nextChar));
            if (nextChar == newline) {
                lineCount++;
            }
            if (input->eof()) {
                std::cout
                    << u8"エラー：文字列を読みながら、ファイルの終わり（ＥＯＦ"
                       u8"）"
                    << endl;
                return MatcherResult(
                    Token(TokenType::END, "", currentLineNumber));
            }
        }
        return MatcherResult(Token(TokenType::STRING, string(resultString),
                                   currentLineNumber + lineCount),
                             lineCount);
    }
    return {};
}
