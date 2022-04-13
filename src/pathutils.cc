#include "pathutils.h"

#include <string>
#include <cstring>
#include <cstdlib>
#ifdef _WIN32
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <libgen.h>
#endif

std::string getDirectoryForPath(const std::string &path) {
#ifdef _WIN32
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    _splitpath( path.c_str(), drive, dir, fname, ext );
    // printf( "Path extracted with _splitpath:\n" );
    // printf( "  Drive: %s\n", drive );
    // printf( "  Dir: %s\n", dir );
    // printf( "  Filename: %s\n", fname );
    // printf( "  Ext: %s\n", ext );

    return std::string(dir);
#else
    char *cdir = strdup(path.c_str());
    std::string dir(dirname(cdir));
    free(cdir);
    return dir;
#endif
}
