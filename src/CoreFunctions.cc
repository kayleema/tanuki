#include "Value.h"
#include "Evaluate.h"
#include "Context.h"
#include "InputSource.h"
#include "Extension.h"

#include <iostream>

const wchar_t *corePinponStarter = 
#include "core.pin"
;

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

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        for (auto value : args) {
            if (value->type == ValueType::NUM) {
                cout << value->toNumberValue()->value;
            } else if (value->type == ValueType::STRING) {
                cout << encodeUTF8(value->toStringValue()->value);
            } else {
                cout << value->toString();
            }
        }
        cout << endl;
        return env->context->newNoneValue();
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

class FunctionNewDictionary : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        auto result = env->context->newDictionaryValue();
        if (!args.empty() && result->type == ValueType::DICT) {
            result->setParent(args[0]->toDictionaryValue());
        }
        return result;
    };
};

class FunctionReadFile : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<wstring, Value *> *) const override {
        if (args.size() != 1) {
            return env->context->newNoneValue();
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
            return env->context->newNoneValue();
        }
        auto moduleEnv = env->newChildEnvironment();
        wstring text = args[0]->toStringValue()->value;
        StringInputSource inputSource(text.c_str());
        FileTokenizer tokenizer(&inputSource);
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
        return env->context->newNoneValue();
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
    env->bind(L"ファイル読む", new FunctionReadFile());
    env->bind(L"評価", new FunctionEval());
    env->bind(L"エキステンション", new FunctionLoadExt());
}
