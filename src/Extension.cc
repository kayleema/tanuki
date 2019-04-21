#include "Value.h"
#include "Evaluate.h"
#include "Context.h"
#include "Extension.h"
#include <dlfcn.h>
#include <iostream>

using namespace std;

void loadDynamic(Environment *env, const char *libname) {
    // libname of the form "libpinpon_dynamic.dylib"
    void *handle = dlopen (libname, RTLD_LAZY);
    if (!handle) {
        cout << "could not load dynlib" << endl;
        return;
    }
    void *symbol = dlsym(handle, "pinponLoadModule");
    char *error = nullptr;
    if ((error = dlerror()) != nullptr)  {
        cout << "could not load symbol : " << error <<  endl;
        return;
    }

    auto functionPointer = (LoadModuleFunction) symbol;

    (*functionPointer)(env);

    // dlclose(handle);
}
