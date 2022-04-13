#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "TextInput/InputSource.h"
#include <memory>

class Filesystem {
  public:
    virtual std::unique_ptr<InputSource>
    getInputSourceForFilename(const std::string &filename) = 0;
};

class FilesystemImpl : public Filesystem {
  public:
    std::unique_ptr<InputSource>
    getInputSourceForFilename(const std::string &filename) override;
};

#endif // FILESYSTEM_H
