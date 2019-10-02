#include "Extension.h"
#include "Context.h"
#include "Environment.h"
#include "Value.h"
#include <iostream>


#ifdef _WIN32
#else
#include <dlfcn.h>
#endif

using namespace std;

void loadDynamic(Environment *env, const char *libname) {
#ifdef _WIN32
#else
    // libname of the form "libpinpon_dynamic.dylib"
    void *handle = dlopen(libname, RTLD_LAZY);
    if (!handle) {
        cout << "could not load dynlib" << endl;
        return;
    }
    void *symbol = dlsym(handle, "pinponLoadModule");
    char *error = nullptr;
    if ((error = dlerror()) != nullptr) {
        cout << "could not load symbol : " << error << endl;
        return;
    }

    auto functionPointer = (LoadModuleFunction)symbol;

    (*functionPointer)(env);

    // dlclose(handle);
#endif
}
