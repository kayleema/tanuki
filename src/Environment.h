#ifndef EVALUATE_H
#define EVALUATE_H

#include <unordered_map>
#include <unordered_set>
#include "Parser.h"
#include "Value.h"
#include "Filesystem.h"

class Context;
class ExitHandler;

class Environment {
    Value *eval_call(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_tail(Value *first, SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_calltail(Value *function, SyntaxNode *tail,
                         const FunctionValue *tailContext = nullptr);

    Value *eval_get(Value *source, SyntaxNode *tree);

    Value *eval_get_bind(Value *source, SyntaxNode *tree);

    Value *eval_subscript(Value *source, SyntaxNode *tree);

    Value *eval_subscript_set(Value *source, SyntaxNode *tree);

    Value *eval_set(Value *source, SyntaxNode *tree);

    Value *eval_terminal(SyntaxNode *node);

    Value *eval_text(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_function(SyntaxNode *node);

    Value *eval_return(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Value *eval_import(SyntaxNode *node);

    Value *eval_nonlocal(SyntaxNode *node);

    Value *eval_if(SyntaxNode *node);

    Value *eval_assign(SyntaxNode *node);

    Value *eval_add(SyntaxNode *node);

    Value *eval_sub(SyntaxNode *node);

    Value *eval_mul(SyntaxNode *node);

    Value *eval_div(SyntaxNode *node);

    Value *eval_equal(SyntaxNode *node);

    Value *eval_not_equal(SyntaxNode *node);

    Value *eval_gt(SyntaxNode *node);

    Value *eval_lt(SyntaxNode *node);

    Value *eval_gte(SyntaxNode *node);

    Value *eval_lte(SyntaxNode *node);

    Value *eval_assert(SyntaxNode *node);

public:
    explicit Environment(Context *context, Filesystem *_filesystem = nullptr, PinponLogger *_logger = nullptr);

    explicit Environment(Environment *parent, Context *_context = nullptr)
            : parent(parent), filesystem(parent->filesystem), logger(parent->logger) {
        exitHandler = parent->exitHandler;
        if (_context == nullptr) {
            if (parent->context == nullptr) {
                cout << "ERROR ERROR null parent context" << endl;
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
    unordered_set<wstring> nonlocals;
    Filesystem *filesystem;
    PinponLogger *logger;
    ExitHandler *exitHandler = nullptr;

    Value *lookup(const wstring &name);

    void bind(const wstring &name, Value *value, bool recursive=false);

    Value *eval(SyntaxNode *node, const FunctionValue *tailContext = nullptr);

    Environment *newChildEnvironment();

    void tailReset() {
        bindings.clear();
    }

    DictionaryValue *toNewDictionaryValue();
};

class ExitHandler {
    public:
    virtual void handleExit() = 0;
};

#endif
