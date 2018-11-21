#include <iostream>
#include "parser.h"

using namespace std;

int main(int argc, char *argv[]) {
	
	//Checking the n° of arguments.
	if(argc != 2){
		cout << "Invalid arguments\n";
		return 1;
	}

	//Checking the file extension.
	if(string(argv[1]).find(".odbdp") == string::npos){
		cout << "Need an .odbdp file\n";
		return 1;	
	}
	
    Parser *parser = new Parser();
    parser->parse(argv[1]);
    return 0;
}
