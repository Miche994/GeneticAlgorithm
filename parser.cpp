#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include "parser.h"

using namespace std;

Parser::Parser() {
}

void Parser::parse(const string filename) {
  ifstream readFile(filename);

  if (readFile.is_open())
  {
    while ( getline(readFile,line) )
    {
      cout << line << '\n';
    }
    readFile.close();
  }
  else 
  {
    cout << "File doesn't exist or doesn't have read privilege\n";
  }
  return;
}
