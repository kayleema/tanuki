#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include <unordered_set>
#include "Parser.h"
#include "Value.h"
#include "Filesystem.h"

class Context;

class Environment {
    Value *eval_call(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_tail(Value *first, SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_calltail(FunctionValue *function, SyntaxNode *node,
                         const FunctionValue *tailContext = nullptr);

    Value *eval_get(DictionaryValue *source, SyntaxNode *tree);

    Value *eval_set(DictionaryValue *source, SyntaxNode *tree);

    Value *eval_terminal(SyntaxNode *node);

    Value *eval_text(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_function(SyntaxNode *node);

    Value *eval_return(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_import(SyntaxNode *node);

    Value *eval_if(SyntaxNode *node);

    Value *eval_assign(SyntaxNode *node);

public:
    explicit Environment(Context *context, Filesystem *_filesystem = nullptr);

    explicit Environment(Environment *parent, Context *_context = nullptr)
            : parent(parent), filesystem(parent->filesystem) {
        if (_context == nullptr) {
            if (parent->context == nullptr) {
                cout << "error null parent context" << endl;
            }
            context = parent->context;
        } else {
            context = _context;
        }
    }

    Environment *parent = nullptr;
    Environment *caller = nullptr;
    Context *context;
    unordered_map<wstring, Value *> bindings;
    Filesystem *filesystem;

    Value *lookup(const wstring &name);

    void bind(const wstring &name, Value *value);

    Value *eval(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Environment *newChildEnvironment();

    void tailReset() {
        bindings.clear();
    }

    DictionaryValue *toNewDictionaryValue();
};

#endif
