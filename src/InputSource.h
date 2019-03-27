#ifndef INPUT_SOURCE_H
#define INPUT_SOURCE_H
#include <fstream>

using namespace std;

class InputSource {
public:
	virtual wchar_t getChar() = 0;

	virtual wchar_t peekChar() = 0;

	virtual bool eof() = 0;
};

class FileInputSource : public InputSource {
	wifstream file;
public:
	FileInputSource(char* filename);

	wchar_t getChar() override;

	wchar_t peekChar() override;

	bool eof() override;
};

class StringInputSource : public InputSource {
	const wchar_t* source;
	bool finished;
public:
	StringInputSource(const wchar_t* source) : source(source), finished(false) {};

	wchar_t getChar() override;

	wchar_t peekChar() override;

	bool eof() override;
};

#endif