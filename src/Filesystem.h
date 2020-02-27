#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "InputSource.h"
#include <memory>

class Filesystem {
public:
    virtual unique_ptr<InputSource>
    getInputSourceForFilename(const string &filename) = 0;
};

class FilesystemImpl : public Filesystem {
public:
    unique_ptr<InputSource>
    getInputSourceForFilename(const string &filename) override;
};

#endif //FILESYSTEM_H
