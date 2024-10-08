#include "Value.h"
#include "Environment.h"
#include "Context.h"
#include "TextInput/FileInputSource.h"
#include "TextInput/StringInputSource.h"
#include "Extension.h"
#include "CoreFunctions.h"
#include "Logger.h"
#include "Lexer/Tokenizer.h"
#include "TextInput/UnicodeConversion.h"

#include "core.tnk"


// Builtin Functions
class FunctionSum : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        long result = 0;
        for (auto value : args) {
            if (value->type != ValueType::NUM) {
                return Context::newNoneValue();
            }
            result += value->toNumberValue()->value;
        }
        return env->context->newNumberValue(result);
    };
};

class FunctionDiff : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
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
                 unordered_map<string, Value *> *) const override {
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
                 unordered_map<string, Value *> *) const override {
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
    FunctionPrint() {}

    ~FunctionPrint() override {}

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        for (auto value : args) {
            if (value->type == ValueType::NUM) {
                env->logger->logLong(value->toNumberValue()->value);
            } else if (value->type == ValueType::STRING) {
                env->logger->log((value->toStringValue()->value));
            } else {
                env->logger->log(value->toString());
            }
        }
        env->logger->logEndl();
        return Context::newNoneValue();
    };
};

class FunctionEqual : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        if (args[0]->equals(args[1])) {
            return env->context->newNumberValue(1);
        }
        return env->context->newNumberValue(0);
    };
};

class FunctionCompare : public FunctionValue {
public:

    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
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
                                    unordered_map<string, Value *> *kwargs) const {
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
                              unordered_map<string, Value *> *) const {
    if (args.size() == 2 && args[1]->type == ValueType::FUNC) {
        if (args[0]->type == ValueType::DICT) {
            auto dictionary = args[0]->toDictionaryValue();
            auto function = (FunctionValue *) args[1];
            vector<string> keys;
            for (const auto &item : dictionary->value) {
                keys.push_back(item.first);
            }
            for (const auto &key : keys) {
                auto value = dictionary->value[key];
                function->apply({env->context->newStringValue(key), value}, env);
            }
            return Context::newNoneValue();
        } else if (args[0]->type == ValueType::ARRAY) {
            auto array = (ArrayValue *) args[0];
            auto function = (FunctionValue *) args[1];
            vector<Value *> keys;
            for (const auto &item : array->value) {
                keys.push_back(item);
                item->refs++;
            }
            for (const auto &key : keys) {
                function->apply({key}, env);
                key->refs--;
            }
            return Context::newNoneValue();
        } else if (args[0]->type == ValueType::STRING) {
            auto argStringIter = args[0]->toStringValue()->value.c_str();
            auto function = (FunctionValue *) args[1];
            vector<string> keys;
            while (*argStringIter != 0) {
                TnkChar current = getFirstUtfChar(argStringIter);
                keys.push_back(tnkCharToString(current));
                argStringIter += getUtfCharSize(*argStringIter);
            }
            for (const auto &key : keys) {
                function->apply({env->context->newStringValue(key)}, env);
            }
            return Context::newNoneValue();
        } else if (args[0]->type == ValueType::NUM) {
            auto limit = args[0]->toNumberValue()->value;
            auto function = (FunctionValue *) args[1];
            for (long i = 0; i < limit; i++) {
                function->apply({env->context->newNumberValue(i)}, env);
            }
            return Context::newNoneValue();
        }
    }
    return Context::newNoneValue();
}

//class FunctionReadFile : public FunctionValue {
//public:
//    Value *apply(const vector<Value *> &args, Environment *env,
//                 unordered_map<string, Value *> *) const override {
//        if (args.size() != 1) {
//            return Context::newNoneValue();
//        }
//        string fileName = args[0]->toStringValue()->value;
//        FileInputSource fileInputSource(encodeUTF8(fileName).c_str());
//        string sourceCode;
//        wchar_t newChar;
//        while ((newChar = fileInputSource.getChar()) != -1) {
//            sourceCode.push_back(newChar);
//        }
//        return env->context->newStringValue(sourceCode);
//          return env->context->newStringValue(L"");//TODO：未実装
//    };
//};

class FunctionEval : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        if (args.size() != 1) {
            return Context::newNoneValue();
        }
        auto moduleEnv = env->newChildEnvironment();
        string text = args[0]->toStringValue()->value;
        StringInputSource inputSource(text.c_str());
        TanukiTokenizer tokenizer(&inputSource);
        ConsoleLogger logger;
        Parser parser(&logger);
        SyntaxNode *ast = parser.run(tokenizer.getAllTokens());
        moduleEnv->eval(ast);
        delete ast;
        return moduleEnv->toNewDictionaryValue();
    };
};

class FunctionLoadExt : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        string name = args[0]->toStringValue()->value;
        loadDynamic(env, name.c_str());
        return Context::newNoneValue();
    };
};

Value *LengthFunction::apply(const vector<Value *> &args, Environment *env,
             unordered_map<string, Value *> *) const {
    if (args[0]->type == ValueType::ARRAY) {
        auto *array = (ArrayValue *)(args[0]);
        return env->context->newNumberValue(array->length());
    } else if (args[0]->type == ValueType::STRING) {

        return env->context->newNumberValue(
            getCodepointLength(args[0]->toStringValue()->value.c_str()));
    } else {
        return Context::newNoneValue();
    }
};

class IndexFunction : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<string, Value *> *) const override {
        auto *array = (ArrayValue *) (args[0]);
        auto *index = (NumberValue *) (args[1]);
        return array->getIndex(index->value);
    };
};

class ArrayUpdate : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *,
                 unordered_map<string, Value *> *) const override {
        auto *array = (ArrayValue *) (args[0]);
        auto *index = (NumberValue *) (args[1]);
        auto newValue = args[2];
        array->set(index->value, newValue);
        return newValue;
    };
};

class ArrayAdd : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *e,
                 unordered_map<string, Value *> *) const override {
        if (args[0]->type != ValueType::ARRAY ) {
            return e->context->newNoneValue();
        }
        auto *array = (ArrayValue *) (args[0]);
        auto *newValue = args[1];
        array->push(newValue);
        return array;
    };
};

//class ArrayInsert : public FunctionValue {
//public:
//    Value *apply(const vector<Value *> &args, Environment *,
//                 unordered_map<string, Value *> *) const override {
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
                 unordered_map<string, Value *> *) const override {
        auto *dict = args[0]->toDictionaryValue();
        StringValue *key = args[1]->toStringValue();
        return dict->get(key->value);
    };
};

class ModFunction : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        auto *arg1 = args[0];
        auto *arg2 = args[1];
        return env->context->newNumberValue(arg1->toNumberValue()->value % arg2->toNumberValue()->value);
    };
};

class FunctionSetParent : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        auto *arg0 = args[0];
        auto *arg1 = args[1];
        if (!(arg0->type == ValueType::ARRAY || arg0->type == ValueType::DICT) ||
            arg1->type != ValueType::DICT) {
            // TODO: log error
            return Context::newNoneValue();
        }
        auto *dict = dynamic_cast<DictionaryValue *> (arg0);
        auto *parentDict = dynamic_cast<DictionaryValue *> (arg1);
        dict->setParent(parentDict);
        return env->context->newNoneValue();
    };
};

class FunctionGetType : public FunctionValue {
public:
    Value *apply(const vector<Value *> &args, Environment *env,
                 unordered_map<string, Value *> *) const override {
        auto *arg0 = args[0];
        return arg0->getLookupSource(env);
    };
};

void initModule(Environment *env) {
    env->bind("足す", new FunctionSum());
    env->bind("引く", new FunctionDiff());
    env->bind("割り算", new FunctionDiv());
    env->bind("掛ける", new FunctionMul());
    env->bind("表示", new FunctionPrint());
    env->bind("イコール", new FunctionEqual());
    env->bind("比べ", new FunctionCompare());
    env->bind("剰余", new ModFunction());
    env->bind("辞書", new FunctionNewDictionary());
    env->bind("新種類", new FunctionNewDictionary());
    env->bind("親設定する", new FunctionSetParent());
    env->bind("それぞれ", new FunctionForEach());
    env->bind("長さ", new LengthFunction());
    env->bind("配列調べ", new IndexFunction());
    env->bind("配列更新", new ArrayUpdate());
    env->bind("配列追加", new ArrayAdd());
    env->bind("辞書調べ", new DictLookup());
    env->bind("何型", new FunctionGetType());
//    env->bind("ファイル読む", new FunctionReadFile());
    env->bind("評価", new FunctionEval());
    env->bind("エキステンション", new FunctionLoadExt());
}
