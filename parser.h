#ifndef __PARSER__
#define __PARSER__

#include <regex>
#include <string>
#include "database.h"

using namespace std;

class Parser {

public:
	Parser();
	void parse(const string filename, Database *db);
private:
	string line;
	regex myRegex;
};

#endif
