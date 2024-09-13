#include "Runtime/Value.h"

class MockFunction : public FunctionValue {
  public:
    Value *
    apply(const vector<Value *> &, Environment *,
          unordered_map<std::string, Value *> * = nullptr) const override {
        return Context::newNoneValue();
    }
};
