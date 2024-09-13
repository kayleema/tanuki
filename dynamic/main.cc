#include "main.h"
#include "Runtime/Context.h"
#include <iostream>

void tanukiLoadModule(Environment *env) {
	cout << "nativeソースコードで表示します。" << endl;
	env->bind("あ", env->context->newStringValue("この文字列はnativeソースコードで定義されました。"));
}
