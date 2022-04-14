#include "TokenProcessor.h"

vector<Token> IndentationTokenProcessor::process(const vector<Token> &input) {
    auto lines = vector<vector<Token>>();
    lines.emplace_back();
    for (auto &token : input) {
        lines.back().push_back(token);
        if (lines.back().back().type == TokenType::NEWL) {
            lines.emplace_back();
        }
    } while (input.back().type != TokenType::END);

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
