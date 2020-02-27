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
    } else if ((result = run_function())) {
        return result;
    } else if ((result = run_assign())) {
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
        expect(TokenType::COMMA);
        SyntaxNode *rhs = run_infix_expression();
        auto result = new SyntaxNode(NodeType::ASSERT, {
                rhs
        });
        result->content = assertToken;
        return result;
    }
    return nullptr;
}

SyntaxNode *Parser::run_assign() {
    Token lhs;
    if (accept({TokenType::SYMBOL, TokenType::ASSIGN}, {&lhs, nullptr})) {
        auto rhs = run_infix_expression();
        auto result = new SyntaxNode(NodeType::ASSIGN);
        result->children.push_back(new SyntaxNode(lhs));
        result->children.push_back(rhs);
        return result;
    }
    return nullptr;
}

SyntaxNode *Parser::run_function() {
    if (!accept(TokenType::FUNC)) {
        return nullptr;
    }
    if (!expect(TokenType::COMMA)) {
        return nullptr;
    }
    Token name;
    accept(TokenType::SYMBOL, &name);
    auto result = new SyntaxNode(NodeType::FUNC);
    result->children.push_back(new SyntaxNode(name));
    if (!expect(TokenType::LPAREN)) {
        return nullptr;
    }
    auto params = new SyntaxNode(NodeType::PARAMS);
    result->children.push_back(params);
    while (!accept(TokenType::RPAREN)) {
        Token param;
        if (accept(TokenType::SYMBOL, &param)) {
            if (accept(TokenType::COLON)) {
                auto rhs = run_infix_expression();
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
        }
        if (accept(TokenType::STAR)) {
            if (accept(TokenType::STAR)) {
                if (accept(TokenType::SYMBOL, &param)) {
                    // Variable-length Keyword Parameter
                    auto kwargs = new SyntaxNode(NodeType::VARKWPARAM);
                    kwargs->children.push_back(new SyntaxNode(param));
                    params->children.push_back(kwargs);
                    accept(TokenType::COMMA);
                }
            } else if (accept(TokenType::SYMBOL, &param)) {
                // Variable-length  Parameter
                auto varparam = new SyntaxNode(NodeType::VARPARAM);
                varparam->children.push_back(new SyntaxNode(param));
                params->children.push_back(varparam);
                accept(TokenType::COMMA);
            }
        }
    }
    if (!expect(TokenType::NEWL) || !expect(TokenType::INDENT)) {
        return nullptr;
    }
    result->children.push_back(run_text());
    if (!expect(TokenType::DEDENT)) {
        return nullptr;
    }
    return result;
}

SyntaxNode *Parser::run_infix_expression() {
    return run_infix_comparison_expression();
}

SyntaxNode *Parser::run_infix_comparison_expression() {
    auto lhs = run_infix_additive_expression();
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
            return new SyntaxNode(pair.second, {lhs, rhs});
        }
    }
    return lhs;
}

SyntaxNode *Parser::run_infix_additive_expression() {
    auto result = run_expression();
    do {
        if (accept(TokenType::MINUS)) {
            auto rhs = run_expression();
            if (!rhs) {
                cout << "error: expected rhs for minus" << endl;
                return nullptr;
            }
            result = new SyntaxNode(NodeType::SUB, {result, rhs});
        } else if (accept(TokenType::PLUS)) {
            auto rhs = run_expression();
            if (!rhs) {
                cout << "error: expected rhs for plus" << endl;
                return nullptr;
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
            auto result = new SyntaxNode(NodeType::CALL);
            result->children.push_back(new SyntaxNode(start));
            result->children.push_back(tail);
            return result;
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
        }
        return nullptr;
    }
    return nullptr;
}

SyntaxNode *Parser::run_expression_tail() {
    if (accept(TokenType::LBRACE)) {
        SyntaxNode *node = nullptr;
        auto arg = run_infix_expression();
        if (!accept(TokenType::RBRACE)) {
            cout << "エラー：パーシング：「　】　」は見つけられなかった！　" << node->toString() << endl;
        }
        if (accept(TokenType::ASSIGN)) {
            auto rhs = run_infix_expression();
            node = new SyntaxNode(NodeType::SUBSCRIPT_SET, {arg, rhs});
        } else {
            node = new SyntaxNode(NodeType::SUBSCRIPT, {arg});
            auto tail = run_expression_tail();
            if (tail) {
                node->children.push_back(tail);
            }
        }
        return node;
    }
    if (accept(TokenType::LPAREN)) {
        auto node = new SyntaxNode(NodeType::CALL_TAIL);
        auto args = run_args();
        if (!accept(TokenType::RPAREN)) {
            cout << "error　" << node->toString() << endl;
        }
        auto tail = run_expression_tail();
        node->children.push_back(args);
        if (tail) {
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
            node->children.push_back(tail);
        }
        return node;
    }
    if (accept(
            {TokenType::DOT, TokenType::SYMBOL, TokenType::ASSIGN},
            {nullptr, &symbol, nullptr})) {
        auto node = new SyntaxNode(NodeType::SET);
        node->children.push_back(new SyntaxNode(symbol));
        auto rhs = run_infix_expression();
        if (rhs) {
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
                auto *arg = new SyntaxNode(NodeType::KWARG);
                arg->children.push_back(lhs);
                arg->children.push_back(rhs);
                node->children.push_back(arg);
            } else {
                auto *arg = run_infix_expression();
                node->children.push_back(arg);
            }
        } while (accept(TokenType::COMMA));
    }
    return node;
}

bool Parser::accept(TokenType type, Token *out) {
    if (currentToken().type == type) {
        if (out) {
            *out = currentToken();
        }
        currentTokenIndex++;
        return true;
    }
    return false;
}

bool Parser::accept(const vector<TokenType> &types, const vector<Token *> &outs) {
    size_t i = 0;
    for (auto type : types) {
        size_t tokenIndex = (size_t) currentTokenIndex + i;
        if (tokenIndex >= allTokens.size()) {
            break;
        }
        if (allTokens[tokenIndex].type != type) {
            return false;
        }
        i++;
    }
    i = 0;
    for (auto out : outs) {
        if (out) {
            *out = allTokens[currentTokenIndex + i];
        }
        i++;
    }

    currentTokenIndex += types.size();
    return true;
}

bool Parser::accept(const vector<TokenType> &types, const vector<Token *> &outs, const vector<TokenType> &rejectTypes) {
    int i = 0;
    for (auto rejectType : rejectTypes) {
        size_t tokenIndex = currentTokenIndex + types.size() + i;
        if (tokenIndex >= allTokens.size()) {
            break;
        }
        if (allTokens[tokenIndex].type == rejectType) {
            return false;
        }
        i++;
    }
    return Parser::accept(types, outs);
}

bool Parser::expect(TokenType type) {
    if (accept(type)) {
        return true;
    }
    logInternal("意外なトークン：" + currentToken().toString() + "\n");
    logInternal("  希望のトークン型は「" + string(tokenTypeToString(type)) + "」\n");
    return false;
}

Token Parser::currentToken() {
    return allTokens[currentTokenIndex];
}

string SyntaxNode::toString(int indent) {
    ostringstream result("");

    string indentation((unsigned long) indent, ' ');
    result << indentation;

    result << NodeTypeStrings[(int) type];
    if (type == NodeType::TERMINAL) {
        result << " " << content.toString();
    }
    result << endl;
    for (auto child : children) {
        result << child->toString(indent + 1);
    }
    return result.str();
}

bool SyntaxNode::isError() const {
    return type == NodeType::PARSE_ERROR;
}

bool SyntaxNode::operator==(const SyntaxNode &other) const {
    if (children.size() != other.children.size() || type != other.type) {
        return false;
    }
    if (type == NodeType::TERMINAL && content != other.content) {
        return false;
    }
    for (std::vector<SyntaxNode *>::size_type i = 0; i != children.size(); i++) {
        if ((*children[i]) != (*other.children[i])) {
            return false;
        }
    }
    return true;
}


ostream &operator<<(ostream &out, const SyntaxNode &node) {
    out << "SyntaxNode("
        << NodeTypeStrings[(int) node.type]
        << ", ";
    if (node.type == NodeType::TERMINAL) {
        out << node.content;
    } else {
        out << "{";
        std::string separator;
        for (const auto &child : node.children) {
            out << separator << *child;
            separator = ",";
        }
        out << "}";
    }
    out << ")";
    return out;
}

SyntaxNode::~SyntaxNode() {
    for (auto child : children) {
        delete child;
    }
}

void Parser::logInternal(string message) {
    if (logger) {
        logger->log(message);
    } else {
        cout << message;
    }
}
