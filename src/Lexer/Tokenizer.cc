#include "Tokenizer.h"
#include <utility>

string encodeUTF8(const wstring &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.to_bytes(in);
    } catch (const std::range_error &exception) {
        cout << "could not convert to utf8 string " << in.size() << endl;
        return {"XXX"};
    }
}

wstring decodeUTF8(const string &in) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv1;
        return conv1.from_bytes(in);
    } catch (const std::range_error &exception) {
        return {L"XXX"};
    }
}

vector<Token> Tokenizer::getAllTokens() {
    auto allTokens = vector<Token>();
    do {
        auto newToken = getToken();
        allTokens.push_back(newToken);
    } while (allTokens.back().type != TokenType::END);

    auto lines = vector<vector<Token>>();
    lines.emplace_back();
    for (auto &token : allTokens) {
        lines.back().push_back(token);
        if (lines.back().back().type == TokenType::NEWL) {
            lines.emplace_back();
        }
    } while (allTokens.back().type != TokenType::END);

    vector<Token> result;
    int indentLevel = 0;
    int newIndentLevel = 0;
    bool indentProcessed = false;
    for (auto &line : lines) {
        for (auto &token : line) {
            if (token.type == TokenType::INDENT) {
                newIndentLevel++;
                if (newIndentLevel > indentLevel) {
                    result.emplace_back(TokenType::INDENT, L"", token.line);
                }
            } else {
                if (newIndentLevel < indentLevel && !indentProcessed) {
                    for (int i = 0; i < (indentLevel - newIndentLevel); i++) {
                        result.emplace_back(TokenType::DEDENT, L"", token.line);
                    }
                }
                result.push_back(token);
                if (token.type == TokenType::NEWL) {
                    indentLevel = newIndentLevel;
                    newIndentLevel = 0;
                    indentProcessed = false;
                } else {
                    indentProcessed = true;
                }
            }
        }
    }

    return result;
}

Token InputSourceTokenizer::getToken() {
    wchar_t first = input->getChar();

    for (auto &matcher : matchers) {
        auto match = matcher->match(first, lineNumber, input);
        lineNumber += match.incrementLineNumber;
        if (match.matched) {
            return match.result;
        }
    }

    cout << "ERROR: Tokenizer overrun past end of file. Line " << lineNumber << endl;
    return {};
}
