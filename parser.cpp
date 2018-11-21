#include <iostream>
#include <fstream>
#include <iostream>
#include <regex>
#include "parser.h"

using namespace std;

Parser::Parser() {
  regSingle = ".+ (\\d+)";
}

void Parser::parse(const string filename) {
  ifstream readFile(filename);
  smatch sm;    // same as std::match_results<const char*> cm;
  if (readFile.is_open())
  {
    while ( getline(readFile,line) )
    {
      if (std::regex_search(line, sm, regSingle) && sm.size() > 1) {
        cout << "MATCHATO " << sm.str(1) << "\n";
      } 
    }
    readFile.close();
  }
  else 
  {
    cout << "File doesn't exist or doesn't have read privilege\n";
  }
  return;
}
