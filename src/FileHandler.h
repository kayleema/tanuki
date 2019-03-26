#include <string>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <clocale>
#include <cwchar>


using namespace std;

class Token {
public:
	Token(wstring content, int line) : content(content), line(line) {}

	wstring content;
	int line;
};

class Tokenizer {
public:
	virtual const Token &getToken() = 0;
};

class UnicodeReader {
	ifstream file;
public:
	UnicodeReader(char* filename) : file(filename, ios::binary) {
		file.seekg(0, std::ios::end);
	    size_t len = file.tellg();
	    file.seekg(0);
	    char *c = new char[len+1];
	    // const char * c = u8"z\u00df\u6c34\U0001d10b";
	    // len = strlen(c);
	    file.read(c, len);
	    cout << "read bytes count: " << len << endl;

	    std::mbstate_t state = std::mbstate_t();
	    const char *ptr = c;
	    const char* end = ptr + len;
	    wchar_t wc;
	    int extracted_len;
	    cout << ptr << endl;
	    while((extracted_len = mbrtowc(&wc, ptr, len, &state)) > 0) {
	        cout << "Next " << extracted_len << " bytes are the character " << wc << '\n';
	        ptr += extracted_len;
    	}

		// wchar_t c;
		// mbstate_t state {};
		// while(!wif.get(c).eof()) {
		// 	cout << "item:" << c << endl;
	 //        string mb(10, '\0'); // Unicode representation
  //           size_t ret = wcrtomb(&mb[0], c, &state); // Number of UTF8 bytes
  //           cout << "multibyte char " << mb << " is " << ret << " bytes" << endl;
		// }
	}

};

class FileTokenizer : public Tokenizer {
	UnicodeReader reader;
public:
	FileTokenizer(char* filename) : reader(filename) {
	}
	virtual const Token &getToken() {
		return std::move(Token(L"hello", 1));
	};
};
