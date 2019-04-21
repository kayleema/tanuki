#include "main.h"
#include "Context.h"
#include <iostream>

void pinponLoadModule(Environment *env) {
	cout << "loadmodule" << endl;
	env->bind(L"あ", env->context->newStringValue(L"ホゲホゲホゲホゲホゲホゲ"));
}
