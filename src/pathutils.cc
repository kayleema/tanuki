#include "pathutils.h"

#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <libgen.h>
#endif

std::string getDirectoryForPath(const std::string &path) {
#ifdef _WIN32
    char pa[MAX_PATH] = "";
    strcpy(pa, path.c_str());
    PathRemoveFileSpecA(pa);
    return std::string(pa);
#else
    auto cdir = strdup(path.c_str());
    auto dir = string(dirname(cdir));
    free(cdir);
#endif
}
