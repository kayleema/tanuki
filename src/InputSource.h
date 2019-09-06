#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H

#include <fstream>

using namespace std;

// Abstract base class for raw input
class InputSource {
public:
    virtual ~InputSource() = default;

    virtual wchar_t getChar() = 0;

    virtual wchar_t peekChar() = 0;

    virtual bool eof() = 0;

    virtual bool good() { return true; }
};

class FileInputSource : public InputSource {
    wifstream file;
public:
    explicit FileInputSource(const char *filename);

    ~FileInputSource() override = default;

    wchar_t getChar() override;

    wchar_t peekChar() override;

    bool eof() override;

    bool good() override { return file.good(); }
};

class StringInputSource : public InputSource {
    const wchar_t *source;
    bool finished;
public:
    explicit StringInputSource(const wchar_t *source) : source(source), finished(false) {};

    ~StringInputSource() override = default;

    wchar_t getChar() override;

    wchar_t peekChar() override;

    bool eof() override;
};

#endif