#include "Parser.h"

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
