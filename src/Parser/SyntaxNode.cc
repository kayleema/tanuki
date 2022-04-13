#include "SyntaxNode.h"
#include <sstream>

using namespace std;


SyntaxNode::~SyntaxNode() {
    for (auto child : children) {
        delete child;
    }
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
