#include "Environment.h"
#include "Context.h"
#include "CoreFunctions.h"
#include "pathutils.h"
#include "Logger.h"
#include "Lexer/Tokenizer.h"
#include "TextInput/UnicodeConversion.h"

using namespace std;

Value *Environment::eval(SyntaxNode *tree, const FunctionValue *tailContext) {
    context->collect(this);
    switch (tree->type) {
        case NodeType::CALL:
            return eval_call(tree, tailContext);
        case NodeType::TERMINAL:
            return eval_terminal(tree);
        case NodeType::TEXT:
            return eval_text(tree, tailContext);
        case NodeType::FUNC:
            return eval_function(tree);
        case NodeType::RETURN:
            return eval_return(tree, tailContext);
        case NodeType::IMPORT:
            return eval_import(tree);
        case NodeType::EXTERNAL:
            return eval_nonlocal(tree);
        case NodeType::IF:
            return eval_if(tree);
        case NodeType::ASSIGN:
            return eval_assign(tree);
        case NodeType::ADD:
            return eval_add(tree);
        case NodeType::SUB:
            return eval_sub(tree);
        case NodeType::MUL:
            return eval_mul(tree);
        case NodeType::DIV:
            return eval_div(tree);
        case NodeType::EQUAL:
            return eval_equal(tree);
        case NodeType::NEQ:
            return eval_not_equal(tree);
        case NodeType::GT:
            return eval_gt(tree);
        case NodeType::LT:
            return eval_lt(tree);
        case NodeType::GTE:
            return eval_gte(tree);
        case NodeType::LTE:
            return eval_lte(tree);
        case NodeType::ASSERT:
            return eval_assert(tree);
        default:
            return Context::newNoneValue();
    }
}

Value *Environment::eval_add(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM_FLOAT && rhs->type == ValueType::NUM_FLOAT) {
        return context->newFloatValue(
                ((FloatValue *) lhs)->value + ((FloatValue *) rhs)->value
        );
    } else if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
                lhs->toNumberValue()->value + rhs->toNumberValue()->value);
    } else if (lhs->type == ValueType::STRING && rhs->type == ValueType::STRING) {
        return context->newStringValue(lhs->toStringValue()->value + rhs->toStringValue()->value);
    } else if (lhs->type == ValueType::ARRAY && rhs->type == ValueType::ARRAY) {
        auto result = context->newArrayValue(this);
        auto a = ((ArrayValue *) lhs)->value;
        auto b = ((ArrayValue *) rhs)->value;
        result->value.insert(result->value.end(), a.begin(), a.end());
        result->value.insert(result->value.end(), b.begin(), b.end());
        return result;
    }
//    auto adder = lhs->getLookupSource(this)->get("足す");
//    if (adder && adder->type == ValueType::FUNC) {
//        return adder->toFunctionValue()->apply({lhs, rhs}, this, nullptr);
//    }//TODO: user defined infix evaluation
    return Context::newNoneValue();
}

Value *Environment::eval_sub(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value - rhs->toNumberValue()->value);
    } else if (lhs->type == ValueType::NUM_FLOAT &&
               rhs->type == ValueType::NUM_FLOAT) {
        return context->newFloatValue(
            lhs->toFloatValue()->value - rhs->toFloatValue()->value);
    }
    return context->newNoneValue();
}

Value *Environment::eval_mul(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value * rhs->toNumberValue()->value);
    } else if (lhs->type == ValueType::NUM_FLOAT &&
               rhs->type == ValueType::NUM_FLOAT) {
        return context->newFloatValue(lhs->toFloatValue()->value *
                                      rhs->toFloatValue()->value);
    }
    return context->newNoneValue();
}

Value *Environment::eval_div(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
                lhs->toNumberValue()->value / rhs->toNumberValue()->value);
    } else if (lhs->type == ValueType::NUM_FLOAT &&
               rhs->type == ValueType::NUM_FLOAT) {
        return context->newFloatValue(lhs->toFloatValue()->value /
                                      rhs->toFloatValue()->value);
    }
    return context->newNoneValue();
}

Value *Environment::eval_equal(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    return context->newNumberValue(lhs->equals(rhs) ? 1 : 0);
}

Value *Environment::eval_not_equal(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    return context->newNumberValue(lhs->equals(rhs) ? 0 : 1);
}

Value *Environment::eval_gt(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value > rhs->toNumberValue()->value ? 1 : 0);
    } else if (lhs->type == ValueType::NUM_FLOAT && rhs->type == ValueType::NUM_FLOAT) {
        return context->newNumberValue(
            lhs->toFloatValue()->value > rhs->toFloatValue()->value ? 1 : 0);
    }
    return context->newNoneValue();
}

Value *Environment::eval_lt(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value < rhs->toNumberValue()->value ? 1 : 0);
    } else if (lhs->type == ValueType::NUM_FLOAT && rhs->type == ValueType::NUM_FLOAT) {
        return context->newNumberValue(
            lhs->toFloatValue()->value < rhs->toFloatValue()->value ? 1 : 0);
    }
    return context->newNoneValue();
}

Value *Environment::eval_gte(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value >= rhs->toNumberValue()->value ? 1 : 0);
    } else if (lhs->type == ValueType::NUM_FLOAT && rhs->type == ValueType::NUM_FLOAT) {
        return context->newNumberValue(
            lhs->toFloatValue()->value >= rhs->toFloatValue()->value ? 1 : 0);
    }
    return context->newNoneValue();
}

Value *Environment::eval_lte(SyntaxNode *tree) {
    auto lhs = eval(tree->children[0]);
    context->tempRefIncrement(lhs);
    auto rhs = eval(tree->children[1]);
    context->tempRefDecrement(lhs);
    if (lhs->type == ValueType::NUM && rhs->type == ValueType::NUM) {
        return context->newNumberValue(
            lhs->toNumberValue()->value <= rhs->toNumberValue()->value ? 1 : 0);
    } else if (lhs->type == ValueType::NUM_FLOAT && rhs->type == ValueType::NUM_FLOAT) {
        return context->newNumberValue(
            lhs->toFloatValue()->value <= rhs->toFloatValue()->value ? 1 : 0);
    }
    return context->newNoneValue();
}

Value *Environment::eval_call(SyntaxNode *tree, const FunctionValue *tailContext) {
    Value *first = eval(tree->children[0]);
    context->tempRefIncrement(first);
    SyntaxNode *tail = tree->children[1];

    Value *result = eval_tail(first, tail, tailContext);

    context->tempRefDecrement(first);
    return result;
}

Value *Environment::eval_tail(Value *first, SyntaxNode *tail,
                              const FunctionValue *tailContext) {
    if (tail->type == NodeType::CALL_TAIL) {
        return eval_calltail(first, tail, tailContext);
    } else if (tail->type == NodeType::GET) {
        return eval_get(first, tail);
    } else if (tail->type == NodeType::GET_BIND) {
        return eval_get_bind(first, tail);
    } else if (tail->type == NodeType::SET) {
        return eval_set(first, tail);
    } else if (tail->type == NodeType::SUBSCRIPT) {
        return eval_subscript(first, tail);
    } else if (tail->type == NodeType::SUBSCRIPT_SET) {
        return eval_subscript_set(first, tail);
    } else {
        return context->newNoneValue();
    }
}


Value *Environment::eval_calltail(Value *functionInput, SyntaxNode *tail,
                                  const FunctionValue *tailContext) {
    if (functionInput->type != ValueType::FUNC) {
        logger->log("実行エラー：関数型ではない物を呼べません。")->logEndl();
        return Context::newNoneValue();
    }
    auto function = dynamic_cast<FunctionValue *>(functionInput);
    if (tail->children.size() == 2) {
        tailContext = nullptr;
    }

    SyntaxNode *args_tree = tail->children[0];
    vector<Value *> args;
    unordered_map<string, Value *> kwargsIn;
    for (auto expression : args_tree->children) {
        if (expression->type == NodeType::KWARG) {
            auto *lhs = expression->children[0];
            auto *rhs = expression->children[1];
            auto arg = eval(rhs);
            kwargsIn[lhs->content.content] = arg;
            context->tempRefIncrement(arg);
        } else {
            auto arg = eval(expression);
            context->tempRefIncrement(arg);
            args.push_back(arg);
        }
    }
    if (function == tailContext) {
        for (auto arg : args) {
            context->tempRefDecrement(arg);
        }
        for (const auto& arg : kwargsIn) {
            context->tempRefDecrement(arg.second);
        }
        auto result = new TailCallValue(args);
        if (!kwargsIn.empty()) {
            result->kwArgs = kwargsIn;
            result->hasKwArgs = true;
        }
        return result;
    }
    auto result = function->apply(
            args, this, kwargsIn.empty() ? nullptr : &kwargsIn);
    auto finalResult = result;
    context->tempRefIncrement(result);

    if (tail->children.size() == 2) {
        auto nextTail = tail->children[1];
        finalResult = eval_tail(result, nextTail);
    }

    for (auto arg : args) {
        context->tempRefDecrement(arg);
    }
    for (const auto &kwarg : kwargsIn) {
        context->tempRefDecrement(kwarg.second);
    }
    context->tempRefDecrement(result);
    return finalResult;
}

Value *Environment::eval_get(Value *source, SyntaxNode *tree) {
    string key = tree->children[0]->content.content;
    DictionaryValue *lookupSource = source->getLookupSource(this);
    if (lookupSource == nullptr) {
        logger->log("エラー：「・」の左側はGET出来ない型。")->log(key)->logEndl();
        return Context::newNoneValue();
    }
    if (!lookupSource->has(key)) {
        logger->log("エラー：辞書にキーは入っていない。")->log(key)->logEndl();
        return Context::newNoneValue();
    }
    auto getResult = lookupSource->get(key);
    context->tempRefIncrement(getResult);
    auto result = getResult;
    if (tree->children.size() == 2) {
        result = eval_tail(result, tree->children[1]);
    }
    context->tempRefDecrement(getResult);
    return result;
}

Value *Environment::eval_get_bind(Value *source, SyntaxNode *tree) {
    string key = tree->children[0]->content.content;
    DictionaryValue *lookupSource = source->getLookupSource(this);
    if (lookupSource == nullptr) {
        cout << "実行エラー：波ダッシュの左側はGET出来ない型がある。" << (key) << endl;
        return Context::newNoneValue();
    }
    if (!lookupSource->has(key)) {
        cout << "実行エラー：辞書にキーは入っていない。" << (key) << endl;
        return Context::newNoneValue();
    }
    auto getResult = lookupSource->get(key);
    if (getResult->type != ValueType::FUNC) {
        cout << "実行エラー：波線の右側のタイプは関数ではありません。バインドはできません。" << endl;
        return Context::newNoneValue();
    }
    context->tempRefIncrement(getResult);
    auto boundGetResult = context->newBoundFunctionValue(dynamic_cast<FunctionValue *>(getResult), source);
    context->tempRefIncrement(boundGetResult);
    Value *result = boundGetResult;
    if (tree->children.size() == 2) {
        result = eval_tail(result, tree->children[1]);
    }
    context->tempRefDecrement(boundGetResult);
    context->tempRefDecrement(getResult);
    return result;
}

Value *Environment::eval_subscript(Value *source, SyntaxNode *tree) {
    if (source->type == ValueType::DICT) {
        auto sourceDictionary = (DictionaryValue *) source;
        SyntaxNode *arg = tree->children[0];
        string key = ((StringValue *) eval(arg))->value;
        if (!sourceDictionary->has(key)) {
            return Context::newNoneValue();
        }
        auto result = sourceDictionary->get(key);
        if (tree->children.size() == 2) {
            context->tempRefIncrement(result);
            result = eval_tail(result, tree->children[1]);
            context->tempRefDecrement(result);
        }
        return result;
    } else if (source->type == ValueType::ARRAY) {
        auto sourceArray = (ArrayValue *) source;
        SyntaxNode *arg = tree->children[0];
        long index = ((NumberValue *) eval(arg))->value;
        if (sourceArray->length() <= index) {
            cout << "添字は配列の外　添字：" << index << "　長さ：" << sourceArray->length() << endl;
            return Context::newNoneValue();
        }
        auto result = sourceArray->getIndex(index);
        if (tree->children.size() == 2) {
            result = eval_tail(result, tree->children[1]);
        }
        return result;
    } else if (source->type == ValueType::STRING) {
        // TODO: strengthen testing of string subscript
        SyntaxNode *arg = tree->children[0];
        long index = ((NumberValue *) eval(arg))->value;
        auto sourceString = source->toStringValue()->value;
        auto sourceStringIteration = sourceString.c_str();
        TnkChar currentChar;
        for (int i = 0; i <= index; i++) {
            currentChar = getFirstUtfChar(sourceStringIteration);
            if (currentChar == 0) {
                cout << "添字は文字列の外　添字：" << index << "　長さ：" << sourceString.length() << endl;
                return Context::newNoneValue();
            }
            sourceStringIteration += getUtfCharSize(*sourceStringIteration);
        }
        Value *result = context->newStringValue(tnkCharToString(currentChar));
        if (tree->children.size() == 2) {
            context->tempRefIncrement(result);
            result = eval_tail(result, tree->children[1]);
            context->tempRefDecrement(result);
        }
        return result;
    }
    return Context::newNoneValue();
}

Value *Environment::eval_subscript_set(Value *source, SyntaxNode *tree) {
    if (source->type == ValueType::DICT) {
        auto sourceDictionary = (DictionaryValue *) source;
        SyntaxNode *arg = tree->children[0];
        string key = ((StringValue *) eval(arg))->value;
        auto rhs = eval(tree->children[1]);
        sourceDictionary->set(key, rhs);
    } else if (source->type == ValueType::ARRAY) {
        auto sourceArray = (ArrayValue *) source;
        SyntaxNode *arg = tree->children[0];
        long index = ((NumberValue *) eval(arg))->value;
        if (sourceArray->length() <= index) {
            cout << "添字は配列の外　添字：" << index << "　長さ：" << sourceArray->length() << endl;
            return Context::newNoneValue();
        }
        auto rhs = eval(tree->children[1]);
        sourceArray->set(index, rhs);
    }
    return Context::newNoneValue();
}

Value *Environment::eval_set(Value *source, SyntaxNode *tree) {
    if (source->type != ValueType::DICT) {
        cout << "実行エラー：「・ ＝」のときに「＝」の左側はSET出来ない型です。" << source->toString() << endl;
        return Context::newNoneValue();
    }
    auto *sourceDict = dynamic_cast<DictionaryValue*>(source);
    string key = tree->children[0]->content.content;
    auto rhs = eval(tree->children[1]);
    sourceDict->set(key, rhs);
    return Context::newNoneValue();
}

Value *Environment::eval_terminal(SyntaxNode *tree) {
    if (tree->content.type == TokenType::SYMBOL) {
        string name = tree->content.content;
        auto result = lookup(name);
        if (!result) {
            return Context::newNoneValue();
        }
        return lookup(name);
    }
    if (tree->content.type == TokenType::NUMBER) {
        auto result = context->newNumberValue(tree->content.number);
        return result;
    }
    if (tree->content.type == TokenType::NUMBER_FLOAT) {
        auto result = context->newFloatValue(tree->content.numberFloat);
        return result;
    }
    if (tree->content.type == TokenType::STRING) {
        auto result = context->newStringValue(tree->content.content);
        return result;
    }
    return Context::newNoneValue();
}

Value *Environment::eval_text(SyntaxNode *tree,
                              const FunctionValue *tailContext) {
    for (auto statement : tree->children) {
        Value *statementReturnValue = eval(statement, tailContext);
        if (statementReturnValue->type == ValueType::RETURN ||
            statementReturnValue->type == ValueType::TAIL_CALL) {
            return statementReturnValue;
        }
    }
    return Context::newNoneValue();
}

Value *Environment::eval_function(SyntaxNode *tree) {
    std::vector<string> params;
    std::unordered_map<string, Value *> paramsWithDefault;
    bool hasKwParam = false;
    bool hasVarParam = false;
    string kwParamName;
    string varParamName;
    for (auto param : tree->children[1]->children) {
        if (param->type == NodeType::TERMINAL) {
            params.push_back(param->content.content);
        } else if (param->type == NodeType::VARKWPARAM) {
            hasKwParam = true;
            kwParamName = param->children[0]->content.content;
        } else if (param->type == NodeType::VARPARAM) {
            hasVarParam = true;
            varParamName = param->children[0]->content.content;
        } else if (param->type == NodeType::DEFAULTPARAM) {
            auto defaultValue = eval(param->children[1], nullptr);
            context->tempRefIncrement(defaultValue);
            paramsWithDefault[param->children[0]->content.content] = defaultValue;
        }
    }
    auto body = tree->children[2];
    auto function = context->newUserFunctionValue(
            params, paramsWithDefault, body, this);
    if (hasKwParam) {
        function->setVarKeywordParam(kwParamName);
    }
    if (hasVarParam) {
        function->setVarParam(varParamName);
    }
    auto nameNode = tree->children[0];
    if (nameNode->type == NodeType::TERMINAL) {
        string name = nameNode->content.content;
        bindings[name] = function;
    } else if (nameNode->children.empty()) {
        return function;
    } else {
        auto name1 = nameNode->children[0]->content.content;
        auto name2 = nameNode->children[1]->content.content;
        Value *name1value = lookup(name1);
        if (name1value->type != ValueType::DICT) {
            cout << "error not of type dict" << endl;
            return Context::newNoneValue();
        }
        auto *dictionary = dynamic_cast<DictionaryValue *>(name1value);
        dictionary->set(name2, function);
    }
    return Context::newNoneValue();
}

Value *Environment::eval_return(SyntaxNode *tree,
                                const FunctionValue *tailContext) {
    auto result = eval(tree->children[0], tailContext);
    if (result->type == ValueType::TAIL_CALL) {
        return result;
    }
    return new ReturnValue(result);
}

Value *Environment::eval_import(SyntaxNode *tree) {
    auto currentFilePath = (lookup("FILE")->toStringValue()->value);
    auto currentFileDir = getDirectoryForPath(currentFilePath);

    // Get relative path
    auto relativePath = string();
    string dirToken;
    for (auto child : tree->children) {
        dirToken = child->content.content;
        relativePath += string("/") + (dirToken);
    }
    relativePath += string(".tnk");

    // Try Current Directory
    string tryPath = currentFileDir + relativePath;
    auto fileInputSource = filesystem->getInputSourceForFilename(tryPath);
    if (!fileInputSource->good()) {
        tryPath = string(GLOBAL_TANUKI_LIB_SEARCH_PATH) + relativePath;
        fileInputSource = filesystem->getInputSourceForFilename(tryPath);
        if (!fileInputSource->good()) {
            logger->log("ERROR: could not import")->logEndl();
            return Context::newNoneValue();
        }
    }

    // Parse and Exec file
    TanukiTokenizer tokenizer(fileInputSource.get());
    Parser parser(logger);
    auto parsedTree = parser.run(tokenizer.getAllTokens());
    auto importEnv = newChildEnvironment();
    importEnv->bind("FILE", context->newStringValue(tryPath));
    importEnv->eval(parsedTree);
    auto module = importEnv->toNewDictionaryValue();
    bind(dirToken, module);
    return Context::newNoneValue();
}

Value *Environment::eval_nonlocal(SyntaxNode *tree) {
    for (auto &child : tree->children) {
        nonlocals.insert(child->content.content);
    }
    return Context::newNoneValue();
}

Value *Environment::eval_if(SyntaxNode *tree) {
    for (size_t i = 0; i < tree->children.size(); i += 2) {
        if (i == tree->children.size() - 1) {
            return eval(tree->children[i]);
        }
        SyntaxNode *condition = tree->children[i];
        SyntaxNode *body = tree->children[i + 1];
        Value *condValue = eval(condition);
        if (condValue->isTruthy()) {
            auto result = eval(body);
            return result;
        }
    }
    return Context::newNoneValue();
}

Value *Environment::eval_assign(SyntaxNode *tree) {
    auto lhs = tree->children[0]->content.content;
    bind(lhs, eval(tree->children[1]));
    return Context::newNoneValue();
}

Value *Environment::eval_assert(SyntaxNode *tree) {
    auto rhs = eval(tree->children[0]);
    if (!rhs->isTruthy()) {
        logger->log("確認エラー終了：")->logLong(tree->content.line)->logLn("行目");
        if (exitHandler != nullptr) {
            exitHandler->handleExit();
            return new ReturnValue(context->newNumberValue(1));
        } else {
            logger->log("exit ")->logLn("now");
            exit(1);
        }
    }
    return Context::newNoneValue();
}

Value *Environment::lookup(const string &name) {
    if (bindings.count(name)) {
        return bindings[name];
    }
    if (parent) {
        return parent->lookup(name);
    }
    logger
        ->log("lookup failure for '")
        ->log(name)
        ->log("'")
        ->logEndl();
    return Context::newNoneValue();
}

bool Environment::isBound(const string &name) const {
    return bindings.count(name) || (parent && parent->isBound(name));
}

void Environment::bind(const string &name, Value *value, bool recursive) {
    if ((recursive && bindings.find(name) == bindings.end()) || (nonlocals.find(name) != nonlocals.end())) {
        if (parent) {
            parent->bind(name, value, true);
        } else {
            logger
                ->log("reached top stack frame for nonlocal '")
                ->log(name)
                ->log("'")
                ->logEndl();
        }
    } else {
        bindings[name] = value;
    }
}

Environment *Environment::newChildEnvironment() {
    return context->newChildEnvironment(this);
}

// Global Environment Constructor
Environment::Environment(Context *context, Filesystem *_filesystem, PinponLogger *_logger)
        : context(context), filesystem(_filesystem), logger(_logger) {
    static ConsoleLogger staticLogger;
    if (logger == nullptr) {
        logger = &staticLogger;
    }
    initModule(this);
}

DictionaryValue *Environment::toNewDictionaryValue() {
    auto result = context->newDictionaryValue();
    for (const auto &binding : bindings) {
        result->set(binding.first, binding.second);
    }
    return result;
}
