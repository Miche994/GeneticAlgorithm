#ifndef __PARSER__
#define __PARSER__

#include <string>
using namespace std;

class Parser {

public:
	Parser();
	void parse(const string filename);
private:
	string line;
};

#endif
