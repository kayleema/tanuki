#include "Filesystem.h"

unique_ptr<InputSource>
FilesystemImpl::getInputSourceForFilename(const string &filename) {
    return unique_ptr<InputSource>(new FileInputSource(filename.c_str()));
}
