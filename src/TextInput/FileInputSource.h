#ifndef FILE_INPUT_SOURCE_H
#define FILE_INPUT_SOURCE_H

#include "InputSource.h"
#include <fstream>

class FileInputSource : public InputSource {
    std::wifstream file;
  public:
    explicit FileInputSource(const char *filename);

    ~FileInputSource() override = default;

    virtual wchar_t getChar() override;

    wchar_t peekChar() override;

    bool eof() override;

    bool good() override { return file.good(); }
};

#endif