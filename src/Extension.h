#include "Environment.h"

typedef void (*LoadModuleFunction)(Environment *env);

void loadDynamic(Environment *env, const char *libname);
