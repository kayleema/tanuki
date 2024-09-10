#ifndef FILE_INPUT_SOURCE_H
#define FILE_INPUT_SOURCE_H

#include "CharDefinitions.h"
#include "InputSource.h"
#include <fstream>

class FileInputSource : public InputSource {
    std::ifstream file;
    bool peeked = false;
    TnkChar peekedChar = 0;
  public:
    explicit FileInputSource(const char *filename);

    ~FileInputSource() override = default;

    TnkChar getChar() override;

    TnkChar peekChar() override;

    bool eof() override;

    bool good() override { return file.good() && !file.bad(); }
};

#endif