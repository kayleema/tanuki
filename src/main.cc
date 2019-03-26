#include <iostream>
#include "FileHandler.h"
#include <fcntl.h>
#include <sys/uio.h>
#include <locale.h>

using namespace std;

int main(int argc, char **argv) 
{
	setlocale(LC_ALL, "");
    std::cout << "始まります" << std::endl;
	if (argc != 2) {
		return 1;
	}
	auto t = FileTokenizer(argv[1]);
    return 0;
}
