#include "Value.h"
#include "Environment.h"
#include "Context.h"
#include "InputSource.h"
#include "Extension.h"
#include "CoreFunctions.h"

#include <iostream>

#include "core.pin"
#include "Logger.h"


// Builtin Functions
class FunctionSum : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        long result = 0;
        for (auto value : args) {
            result += value->toNumberValue()->value;
        }
        return env->context->newNumberValue(result);
    };
};

class FunctionDiff : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        long result = 0;
        bool first = true;
        for (auto value : args) {
            if (first) {
                first = false;
                result = value->toNumberValue()->value;
            } else {
                result -= value->toNumberValue()->value;
            }
        }
        return env->context->newNumberValue(result);
    };
};

class FunctionDiv : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        long result = 0;
        bool first = true;
        for (auto value : args) {
            if (first) {
                first = false;
                result = value->toNumberValue()->value;
            } else {
                result /= value->toNumberValue()->value;
            }
        }
        return env->context->newNumberValue(result);
    };
};

class FunctionMul : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        long result = 0;
        bool first = true;
        for (auto value : args) {
            if (first) {
                first = false;
                result = value->toNumberValue()->value;
            } else {
                result *= value->toNumberValue()->value;
            }
        }
        return env->context->newNumberValue(result);
    };
};

class FunctionPrint : public FunctionValue {
public:
    PinponLogger *logger;

    FunctionPrint() {
        logger = new ConsoleLogger();
    }

    ~FunctionPrint() override {
        delete logger;
    }

    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<wstring, Value *> *) const override {
        for (auto value : args) {
            if (value->type == ValueType::NUM) {
                logger->logLong(value->toNumberValue()->value);
            } else if (value->type == ValueType::STRING) {
                logger->log(value->toStringValue()->value);
            } else {
                logger->log(value->toString());
            }
        }
        logger->logEndl();
        return Context::newNoneValue();
    };
};

class FunctionEqual : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if (args[0]->equals(args[1])) {
            return env->context->newNumberValue(1);
        }
        return env->context->newNumberValue(0);
    };
};

class FunctionCompare : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if ((args[0]->type != ValueType::NUM) ||
            (args[1]->type != ValueType::NUM)) {
            return env->context->newNumberValue(0);
        }
        auto a = args[0]->toNumberValue()->value;
        auto b = args[1]->toNumberValue()->value;
        return env->context->newNumberValue((a > b) - (a < b));
    };
};

Value *FunctionNewDictionary::apply(const vector<Value *> &args,
                                    Environment *env,
                                    unordered_map<wstring, Value *> *kwargs) const {
    auto result = env->context->newDictionaryValue();
    if (!args.empty() && result->type == ValueType::DICT) {
        result->setParent(args[0]->toDictionaryValue());
    }
    if (kwargs) {
        for (auto &arg : *kwargs) {
            result->set(arg.first, arg.second);
        }
    }
    return result;
}

Value *FunctionForEach::apply(const vector<Value *> &args,
                              Environment *env,
                              unordered_map<wstring, Value *> *) const {
    if (args.size() == 2 && args[0]->type == ValueType::DICT && args[1]->type == ValueType::FUNC) {
        auto dictionary = args[0]->toDictionaryValue();
        auto function = (FunctionValue *) args[1];
        // TODO: optimize
        vector<wstring> keys;
        for (const auto &item : dictionary->value) {
            keys.push_back(item.first);
        }
        for (const auto &key : keys) {
            auto value = dictionary->value[key];
            function->apply({env->context->newStringValue(key), value}, env);
        }
        return Context::newNoneValue();
    }
    return Context::newNoneValue();
}

class FunctionReadFile : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if (args.size() != 1) {
            return Context::newNoneValue();
        }
        wstring fileName = args[0]->toStringValue()->value;
        FileInputSource fileInputSource(encodeUTF8(fileName).c_str());
        wstring sourceCode;
        wchar_t newChar;
        while ((newChar = fileInputSource.getChar()) != -1) {
            sourceCode.push_back(newChar);
        }
        return env->context->newStringValue(sourceCode);
    };
};

class FunctionEval : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if (args.size() != 1) {
            return Context::newNoneValue();
        }
        auto moduleEnv = env->newChildEnvironment();
        wstring text = args[0]->toStringValue()->value;
        StringInputSource inputSource(text.c_str());
        InputSourceTokenizer tokenizer(&inputSource);
        Parser parser(&tokenizer);
        SyntaxNode *ast = parser.run();
        moduleEnv->eval(ast);
        delete ast;
        return moduleEnv->toNewDictionaryValue();
    };
};

class FunctionLoadExt : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        wstring name = args[0]->toStringValue()->value;
        loadDynamic(env, encodeUTF8(name).c_str());
        return Context::newNoneValue();
    };
};

class LengthFunction : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if (args[0]->type != ValueType::ARRAY) {
            return Context::newNoneValue();
        }
        auto *array = (ArrayValue *) (args[0]);
        return env->context->newNumberValue(array->length());
    };
};

class IndexFunction : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<wstring, Value *> *) const override {
        auto *array = (ArrayValue *) (args[0]);
        auto *index = (NumberValue *) (args[1]);
        return array->getIndex(index->value);
    };
};

class ArrayUpdate : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<wstring, Value *> *) const override {
        auto *array = (ArrayValue *) (args[0]);
        auto *index = (NumberValue *) (args[1]);
        auto newValue = args[2];
        array->set(index->value, newValue);
        return newValue;
    };
};

//class ArrayInsert : public FunctionValue {
//public:
//    Value *apply(const vector<Value *> &args, Environment *,
//                 unordered_map<wstring, Value *> *) const override {
//        auto *array = (ArrayValue*)(args[0]);
//        auto *index = (NumberValue*)(args[1]);
//        auto newValue = args[2];
//        array->set(index->value, newValue);
//        return newValue;
//    };
//};

class DictLookup : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<wstring, Value *> *) const override {
        auto *dict = args[0]->toDictionaryValue();
        StringValue *key = args[1]->toStringValue();
        return dict->get(key->value);
    };
};

void initModule(Environment *env) {
    env->bind(L"足す", new FunctionSum());
    env->bind(L"引く", new FunctionDiff());
    env->bind(L"割り算", new FunctionDiv());
    env->bind(L"掛ける", new FunctionMul());
    env->bind(L"表示", new FunctionPrint());
    env->bind(L"イコール", new FunctionEqual());
    env->bind(L"比べ", new FunctionCompare());
    env->bind(L"辞書", new FunctionNewDictionary());
    env->bind(L"それぞれ", new FunctionForEach());
    env->bind(L"長さ", new LengthFunction());
    env->bind(L"配列調べ", new IndexFunction());
    env->bind(L"配列更新", new ArrayUpdate());
    env->bind(L"辞書調べ", new DictLookup());
    env->bind(L"ファイル読む", new FunctionReadFile());
    env->bind(L"評価", new FunctionEval());
    env->bind(L"エキステンション", new FunctionLoadExt());
}
