#include <iostream>
#include "parser.h"
#include "database.h"

using namespace std;

int main(int argc, char *argv[]) {
	
	//Checking the n° of arguments.
	if(argc != 3){
		cout << "Invalid arguments\n";
		return 1;
	}

	//Checking the file extension.
	if(string(argv[1]).find(".odbdp") == string::npos){
		cout << "Need an .odbdp file\n";
		return 1;	
	}
	
	Database *db = new Database();
	int seconds = std::stoi (argv[2]);
    Parser *parser = new Parser();
    parser->parse(argv[1], db);
    return 0;
}
