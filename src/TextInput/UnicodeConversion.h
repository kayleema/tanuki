#ifndef UNICODECONVERSION_H
#define UNICODECONVERSION_H

#include <string>
#include "CharDefinitions.h"

std::string tnkCharToString(TnkChar inputChar);

TnkChar getFirstUtfChar(const char *input);

int getUtfCharSize(unsigned char c);

int getCodepointLength(const char *input);

#endif // UNICODECONVERSION_H