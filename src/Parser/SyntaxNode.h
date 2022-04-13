#ifndef SYNTAX_NODE_H
#define SYNTAX_NODE_H
#include "Lexer/Token.h"
#include <string>
#include <vector>

enum class NodeType {
    CALL, TERMINAL, ARGS, CALL_TAIL, TEXT, FUNC, PARAMS,
    RETURN, IF, ASSIGN, GET, SET, IMPORT, VARKWPARAM, KWARG,
    VARPARAM, SUB, ADD, EQUAL, NEQ, LT, GT, LTE, GTE,
    ASSERT, DEFAULTPARAM, SUBSCRIPT, SUBSCRIPT_SET, PARSE_ERROR,
    MUL, DIV, EXTERNAL, GET_BIND, FUNC_NAME
};

const std::string NodeTypeStrings[] = {
    "CALL", "TERMINAL", "ARGS", "CALL_TAIL", "TEXT", "FUNC", "PARAMS",
    "RETURN", "IF", "ASSIGN", "GET", "SET", "IMPORT", "VARKWPARAM", "KWARG",
    "VARPARAM", "SUB", "ADD", "EQUAL", "NEQ", "LT", "GT", "LTE", "GTE",
    "ASSERT", "DEFAULTPARAM", "SUBSCRIPT", "SUBSCRIPT_SET", "PARSE_ERROR",
    "MUL", "DIV", "EXTERNAL", "GET_BIND", "FUNC_NAME"
};

class SyntaxNode;

class SyntaxNode {
  public:
    NodeType type;
    Token content;
    vector<SyntaxNode *> children;

    explicit SyntaxNode(NodeType _type) : type(_type) {}

    explicit SyntaxNode(Token _content) : type(NodeType::TERMINAL), content(std::move(_content)) {}

    explicit SyntaxNode(NodeType _type, vector<SyntaxNode *> _children)
        : type(_type), children(std::move(_children)) {}

    ~SyntaxNode();

    string toString(int indent = 0);

    bool operator==(const SyntaxNode &other) const;

    bool operator!=(const SyntaxNode &other) const { return !(*this == other); };

    bool isError() const;
};

ostream &operator<<(ostream &out, const SyntaxNode &node);

#endif
