#include "main.h"
#include "Context.h"
#include <iostream>

void tanukiLoadModule(Environment *env) {
	cout << "nativeソースコードで表示します。" << endl;
	env->bind(L"あ", env->context->newStringValue(L"この文字列はnativeソースコードで定義されました。"));
}
