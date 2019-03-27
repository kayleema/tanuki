#include <codecvt>
#include "InputSource.h"

FileInputSource::FileInputSource(char* filename) : file(filename) {
	file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
}

wchar_t FileInputSource::getChar() {
	wchar_t first;
	file.get(first);
	return first;
}

wchar_t FileInputSource::peekChar() {
	return (wchar_t) file.peek();
}

bool FileInputSource::eof() {
	return file.eof();
}


wchar_t StringInputSource::getChar() {
	if (*source == L'\0') {
		finished = true;
		return L'\0';
	}
	return *source++;
}

wchar_t StringInputSource::peekChar() {
	return *source;
}

bool StringInputSource::eof() {
	return finished;
}
