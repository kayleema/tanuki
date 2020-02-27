#include <iostream>
#include <unordered_map>
#include "Parser.h"

SyntaxNode *Parser::run() {
    allTokens = lexer->getAllTokens();
    currentTokenIndex = 0;
    return run_text();
}

SyntaxNode *Parser::run_text() {
    auto result = new SyntaxNode(NodeType::TEXT);

    do {
        while (accept(TokenType::NEWL)) {}
        SyntaxNode *statement = run_statement();
        if (!statement) {
            return result;
        } else if (statement->isError()) {
            delete result;
            return statement;
        }
        result->children.push_back(statement);
    } while (true);
}

SyntaxNode *Parser::run_statement() {
    SyntaxNode *result;
    if ((result = run_import())) {
        return result;
    } else if ((result = run_return())) {
        return result;
    } else if ((result = run_if())) {
        return result;
    } else if ((result = run_assert())) {
        return result;
    } else if ((result = run_assign())) {
        return result;
    } else if ((result = run_function())) {
        return result;
    } else {
        return run_infix_expression();
    }
}

SyntaxNode *Parser::run_import() {
    if (accept(TokenType::IMPORT)) {
        auto result = new SyntaxNode(NodeType::IMPORT);
        if (!expect(TokenType::COMMA)) {
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        do {
            Token part;
            accept(TokenType::SYMBOL, &part);
            result->children.push_back(new SyntaxNode(part));
        } while (accept(TokenType::DOT));
        if (result->children.empty()) {
            logInternal("エラー：導入は不完全\n");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        return result;
    }
    return nullptr;
}

SyntaxNode *Parser::run_return() {
    if (accept(TokenType::RETURN)) {
        if (!expect(TokenType::COMMA)) {
            logInternal("エラー：返す文は不完全\n");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        SyntaxNode *rhs = run_infix_expression();
        if (rhs->isError()) {
            logInternal("エラー：返す文の右側をパース出来ません\n");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        } else if (rhs) {
            return new SyntaxNode(NodeType::RETURN, {rhs});
        } else {
            return new SyntaxNode(NodeType::RETURN);
        }
    }
    return nullptr;
}

SyntaxNode *Parser::run_if() {
    if (!accept(TokenType::IF)) {
        return nullptr;
    }
    if (!expect(TokenType::COMMA)) {
        logInternal("エラー：もし文不完全\n");
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    SyntaxNode *condition = run_infix_expression();
    if (!condition || condition->isError()) {
        logInternal("エラー：もし文の右側の引数をパース出来ない。\n");
        return condition;
    }
    if (!expect(TokenType::NEWL) || !expect(TokenType::INDENT)) {
        logInternal("エラー：もし文の一行目の後でnewlineやindentなどはありません。\n");
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    SyntaxNode *body = run_text();
    if (body->isError()) {
        logInternal("エラー：もし文の中に問題が起きました。\n");
        return body;
    }
    if (!expect(TokenType::DEDENT)) {
        logInternal("エラー：もし文の中身の後でunindentは必要です。\n");
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    auto result = new SyntaxNode(NodeType::IF, {condition, body});

    // Elif part
    while (accept(TokenType::ELIF)) {
        if (!expect(TokenType::COMMA)) {
            logInternal("エラー：もし文の「あるいは」の右側で「、」は必要です。\n");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        SyntaxNode *conditionElif = run_infix_expression();
        if (conditionElif->isError()) {
            logInternal("エラー：もし文の「あるいは」の引数の中に問題があります。\n");
            delete result;
            return conditionElif;
        }
        if (!expect(TokenType::NEWL) || !expect(TokenType::INDENT)) {
            logInternal("エラー：もし文の「あるいは」の引数の中に問題があります。\n");
            delete result;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        SyntaxNode *bodyElif = run_text();
        if (bodyElif->isError()) {
            logInternal("エラー：もし文の「あるいは」部分の内側（ボディー）に問題があります。\n");
            delete result;
            return bodyElif;
        }
        if (!expect(TokenType::DEDENT)) {
            logInternal("エラー：もし文の「あるいは」部分の中身の後でunindentは必要です。\n");
            delete result;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        result->children.push_back(conditionElif);
        result->children.push_back(bodyElif);
    }

    // Else part
    if (accept(TokenType::ELSE)) {
        if (!expect(TokenType::NEWL) || !expect(TokenType::INDENT)) {
            logInternal("エラー：もし文の「その他部分」の一行目の後でnewlineやindentなどはありません。\n");
            delete result;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        SyntaxNode *bodyElse = run_text();
        if (bodyElse->isError()) {
            logInternal("エラー：もし文の「その他」部分の内側（ボディー）に問題があります。\n");
            delete result;
            return bodyElse;
        }
        result->children.push_back(bodyElse);
        if (!expect(TokenType::DEDENT)) {
            logInternal("エラー：もし文の「その他」部分の中身の後でunindentは必要です。\n");
            delete result;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
    }
    return result;
}

SyntaxNode *Parser::run_assert() {
    Token assertToken;
    if (accept(TokenType::ASSERT, &assertToken)) {
        if (!expect(TokenType::COMMA)) {
            logInternal("エラー：確認文の後は「、」\n");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        SyntaxNode *rhs = run_infix_expression();
        if (rhs->isError()) {
            logInternal("エラー：確認文の中に問題があります\n");
            return rhs;
        }
        auto result = new SyntaxNode(NodeType::ASSERT, {rhs});
        result->content = assertToken;
        return result;
    }
    return nullptr;
}

SyntaxNode *Parser::run_assign() {
    Token lhs;
    if (accept({TokenType::SYMBOL, TokenType::ASSIGN}, {&lhs, nullptr})) {
        auto rhs = run_infix_expression();
        if (rhs->isError()) {
            logInternal("エラー：変数設定文の右側に問題\n");
            return rhs;
        }
        return new SyntaxNode(NodeType::ASSIGN, {
                new SyntaxNode(lhs), rhs
        });
    }
    return nullptr;
}

SyntaxNode *Parser::run_function() {
    if (!accept(TokenType::FUNC)) {
        return nullptr;
    }
    if (!expect(TokenType::COMMA)) {
        logInternal("エラー：関数作成文の中に問題があります。\n");
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    Token name;
    if (!accept(TokenType::SYMBOL, &name)) {
        logInternal("エラー：関数作成文の中に関数名がありません。\n");
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    auto result = new SyntaxNode(NodeType::FUNC, {new SyntaxNode(name)});
    if (!expect(TokenType::LPAREN)) {
        logInternal("エラー：関数作成文の一行目で「（」を見つけられません。\n");
        delete result;
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    auto params = new SyntaxNode(NodeType::PARAMS);
    result->children.push_back(params);
    while (!accept(TokenType::RPAREN)) {
        Token param;
        if (accept(TokenType::SYMBOL, &param)) {
            if (accept(TokenType::COLON)) {
                auto rhs = run_infix_expression();
                if (rhs->isError()) {
                    logInternal("エラー：関数作成文のキーワード引数の「：」の右側。\n");
                    delete result;
                    return rhs;
                }
                params->children.push_back(
                        new SyntaxNode(NodeType::DEFAULTPARAM, {
                                new SyntaxNode(param),
                                rhs
                        })
                );
            } else {
                params->children.push_back(new SyntaxNode(param));
            }
            accept(TokenType::COMMA);
        } else if (accept(TokenType::STAR)) {
            if (accept(TokenType::STAR)) {
                if (accept(TokenType::SYMBOL, &param)) {
                    // Variable-length Keyword Parameter
                    auto kwargs = new SyntaxNode(NodeType::VARKWPARAM, {new SyntaxNode(param)});
                    params->children.push_back(kwargs);
                    accept(TokenType::COMMA);
                } else {
                    logInternal("エラー：関数作成文のVariable-length Keyword Parameterの問題。\n");
                    delete result;
                    return new SyntaxNode(NodeType::PARSE_ERROR);
                }
            } else if (accept(TokenType::SYMBOL, &param)) {
                // Variable-length  Parameter
                auto varparam = new SyntaxNode(NodeType::VARPARAM);
                varparam->children.push_back(new SyntaxNode(param));
                params->children.push_back(varparam);
                accept(TokenType::COMMA);
            } else {
                logInternal("エラー：関数作成文の星の右に何もありません。\n");
                delete result;
                return new SyntaxNode(NodeType::PARSE_ERROR);
            }
        } else {
            logInternal("エラー：関数作成文の引数一覧の中に変なトークン。\n");
            delete result;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
    }
    if (!expect(TokenType::NEWL) || !expect(TokenType::INDENT)) {
        logInternal("エラー：関数作成文の一行目の後でindentが必要。\n");
        delete result;
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    auto body = run_text();
    result->children.push_back(body);
    if (body->isError()) {
        logInternal("エラー：関数作成文の中身で問題がありました。\n");
        delete result;
        return body;
    }
    if (!expect(TokenType::DEDENT)) {
        logInternal("エラー：関数作成文の後でunindentが必要。\n");
        delete result;
        return new SyntaxNode(NodeType::PARSE_ERROR);
    }
    return result;
}

SyntaxNode *Parser::run_infix_expression() {
    return run_infix_comparison_expression();
}

SyntaxNode *Parser::run_infix_comparison_expression() {
    auto lhs = run_infix_additive_expression();
    if (!lhs) {
        return lhs;
    } else if (lhs->isError()) {
        logInternal("エラー：比べ文の左側のなか。");
        return lhs;
    }
    static const std::vector<std::pair<TokenType, NodeType>> operatorToNode(
            {
                    {TokenType::EQ,  NodeType::EQUAL},
                    {TokenType::NEQ, NodeType::NEQ},
                    {TokenType::LT,  NodeType::LT},
                    {TokenType::GT,  NodeType::GT},
                    {TokenType::GEQ, NodeType::GTE},
                    {TokenType::LEQ, NodeType::LTE}
            });
    for (const auto pair : operatorToNode) {
        if (accept(pair.first)) {
            auto rhs = run_infix_additive_expression();
            if (!rhs) {
                logInternal("エラー：比べ文の右側のなか。");
                delete lhs;
                return new SyntaxNode(NodeType::PARSE_ERROR);
            } else if (rhs->isError()) {
                logInternal("エラー：比べ文の右側のなか。");
                delete lhs;
                return rhs;
            }
            return new SyntaxNode(pair.second, {lhs, rhs});
        }
    }
    return lhs;
}

SyntaxNode *Parser::run_infix_additive_expression() {
    auto result = run_expression();
    if (!result) {
        return result;
    } else if (result->isError()) {
        logInternal("エラー：プラス型文の左側で。");
        return result;
    }
    do {
        if (accept(TokenType::MINUS)) {
            auto rhs = run_expression();
            if (!rhs) {
                logInternal("エラー：マイナスの右側に何もありません。");
                delete result;
                return new SyntaxNode(NodeType::PARSE_ERROR);
            } else if (rhs->isError()) {
                logInternal("エラー：マイナスの右側で問題がありました。");
                delete result;
                return rhs;
            }
            result = new SyntaxNode(NodeType::SUB, {result, rhs});
        } else if (accept(TokenType::PLUS)) {
            auto rhs = run_expression();
            if (!rhs) {
                logInternal("エラー：プラスの右側に何もありません。");
                delete result;
                return new SyntaxNode(NodeType::PARSE_ERROR);
            } else if (rhs->isError()) {
                logInternal("エラー：プラスの右側で問題がありました。");
                delete result;
                return rhs;
            }
            result = new SyntaxNode(NodeType::ADD, {result, rhs});
        } else {
            return result;
        }
    } while (true);
}

SyntaxNode *Parser::run_expression() {
    Token start;
    if (accept(TokenType::STRING, &start)) {
        return new SyntaxNode(start);
    }
    if (accept(TokenType::SYMBOL, &start)) {
        auto tail = run_expression_tail();
        if (tail) {
            if (tail->isError()) {
                return tail;
            }
            return new SyntaxNode(NodeType::CALL, {new SyntaxNode(start), tail});
        } else {
            return new SyntaxNode(start);
        }
    }
    if (accept(TokenType::NUMBER, &start)) {
        return new SyntaxNode(start);
    }
    if (accept(TokenType::MINUS, &start)) {
        if (accept(TokenType::NUMBER, &start)) {
            start.number = -start.number;
            return new SyntaxNode(start);
        } else {
            logInternal("エラー：役に立たないマイナスがあります。");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
    }
    return nullptr;
}

SyntaxNode *Parser::run_expression_tail() {
    if (accept(TokenType::LBRACE)) {
        SyntaxNode *node = nullptr;
        auto arg = run_infix_expression();
        if (!arg) {
            logInternal("エラー：【】ブレースの引数の中に問題がありました。");
            return new SyntaxNode(NodeType::PARSE_ERROR);
        } else if (arg->isError()) {
            logInternal("エラー：【】ブレースの引数の中に問題がありました。");
            return arg;
        }
        if (!accept(TokenType::RBRACE)) {
            logInternal("エラー：【】ブレースの引数の中に問題がありました。");
            delete arg;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        if (accept(TokenType::ASSIGN)) {
            auto rhs = run_infix_expression();
            if (!rhs) {
                logInternal("エラー：【】ブレースの右側の問題がありました。");
                return new SyntaxNode(NodeType::PARSE_ERROR);
            } else if (rhs->isError()) {
                logInternal("エラー：【】ブレースの右側の問題がありました。");
                return rhs;
            }
            return new SyntaxNode(NodeType::SUBSCRIPT_SET, {arg, rhs});
        } else {
            node = new SyntaxNode(NodeType::SUBSCRIPT, {arg});
            auto tail = run_expression_tail();
            if (tail) {
                if (tail->isError()) {
                    logInternal("エラー：【】ブレースの右側の他のパターンの問題がありました。");
                    delete node;
                    return tail;
                }
                node->children.push_back(tail);
            }
            return node;
        }
    }
    if (accept(TokenType::LPAREN)) {
        auto node = new SyntaxNode(NodeType::CALL_TAIL);
        auto args = run_args();
        if (!args) {
            delete node;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        } else if (args->isError()) {
            logInternal("エラー：関数実行分の渡し方の問題。");
            delete node;
            return args;
        }
        if (!accept(TokenType::RPAREN)) {
            logInternal("エラー：関数実行分で右かっこがありません。" + node->toString());
            delete node;
            return new SyntaxNode(NodeType::PARSE_ERROR);
        }
        auto tail = run_expression_tail();
        node->children.push_back(args);
        if (tail) {
            if (tail->isError()) {
                logInternal("エラー：関数実行分の右側。");
                delete node;
                return tail;
            }
            node->children.push_back(tail);
        }
        return node;
    }
    Token symbol;
    if (accept({TokenType::DOT, TokenType::SYMBOL}, {nullptr, &symbol}, {TokenType::ASSIGN})) {
        auto node = new SyntaxNode(NodeType::GET);
        node->children.push_back(new SyntaxNode(symbol));
        auto tail = run_expression_tail();
        if (tail) {
            if (tail->isError()) {
                logInternal("エラー：ドット調べるの右のパターンで問題あった。");
                delete node;
                return tail;
            }
            node->children.push_back(tail);
        }
        return node;
    }
    if (accept(
            {TokenType::DOT, TokenType::SYMBOL, TokenType::ASSIGN},
            {nullptr, &symbol, nullptr})) {
        auto node = new SyntaxNode(NodeType::SET, {new SyntaxNode(symbol)});
        auto rhs = run_infix_expression();
        if (rhs) {
            if (rhs->isError()) {
                logInternal("エラー：ドット設定するの右で問題ある。");
                delete node;
                return rhs;
            }
            node->children.push_back(rhs);
        }
        return node;
    }
    return nullptr;
}

SyntaxNode *Parser::run_args() {
    auto node = new SyntaxNode(NodeType::ARGS);
    if (currentToken().type == TokenType::RPAREN) {
        return node;
    } else {
        do {
            Token kwSymbol;
            if (accept({TokenType::SYMBOL, TokenType::COLON}, {&kwSymbol, nullptr})) {
                auto *lhs = new SyntaxNode(kwSymbol);
                auto *rhs = run_infix_expression();
                if (!rhs) {
                    logInternal("エラー：関数実行引数一覧での問題。");
                    delete lhs;
                    delete node;
                    return new SyntaxNode(NodeType::PARSE_ERROR);
                } else if (rhs->isError()) {
                    logInternal("エラー：関数実行引数一覧での問題。");
                    delete lhs;
                    delete node;
                    return rhs;
                }
                auto *arg = new SyntaxNode(NodeType::KWARG, {lhs, rhs});
                node->children.push_back(arg);
            } else {
                auto *arg = run_infix_expression();
                if (!arg) {
                    logInternal("エラー：関数実行引数一覧での問題。");
                    delete arg;
                    delete node;
                    return new SyntaxNode(NodeType::PARSE_ERROR);
                } else if (arg->isError()) {
                    logInternal("エラー：関数実行引数一覧での問題。");
                    delete arg;
                    delete node;
                    return arg;
                }
                node->children.push_back(arg);
            }
        } while (accept(TokenType::COMMA));
    }
    return node;
}
