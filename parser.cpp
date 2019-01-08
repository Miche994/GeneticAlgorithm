#include <iostream>
#include <fstream>
#include <iostream>
#include <regex>
#include "parser.h"

using namespace std;

Parser::Parser() {
  this->myRegex = "(\\d+)";
}

void Parser::parse(const string filename, Database *db) {
  ifstream readFile(filename);
  smatch sm;
  if (readFile.is_open())
  {
    //Match and store nQueries
    getline(readFile,line);
    std::regex_search(line, sm, myRegex);
    db->nQueries = std::stoi(sm.str(1).c_str());
    //Match and Store nIndexes
    getline(readFile,line);
    std::regex_search(line, sm, myRegex);
    db->nIndexes = std::stoi(sm.str(1).c_str());
    //Match and store nConfigurations
    getline(readFile,line);
    std::regex_search(line, sm, myRegex);
    db->nConfigurations = std::stoi(sm.str(1).c_str());
    //Match and store totalMemory
    getline(readFile,line);
    std::regex_search(line, sm, myRegex);
    db->totalMemory = std::stoi(sm.str(1).c_str());

    //Match and store configurationIndexMatrix
    getline(readFile,line);
    db->configurationIndexMatrix = new bool*[db->nConfigurations];
    for (int i = 0; i < db->nConfigurations; i++)
      db->configurationIndexMatrix[i] = new bool[db->nIndexes];
    for(int i = 0; i < db->nConfigurations; i++){
      getline(readFile,line);
      int j = 0;
      string::const_iterator searchStart( line.cbegin() );
      while ( std::regex_search( searchStart, line.cend(), sm, myRegex ) ) {
        searchStart = sm.suffix().first;
        if(sm[0] == "1")
          db->configurationIndexMatrix[i][j] = true;
        else
          db->configurationIndexMatrix[i][j] = false;
        j++;
      }
    }
    
    //Match and store indexesCost
    getline(readFile,line);
    db->indexesCost = new int[db->nIndexes];
    for(int i = 0; i < db->nIndexes; i++) {
      getline(readFile,line);
      std::regex_search(line, sm, myRegex);
      db->indexesCost[i] = std::stoi(sm.str(1).c_str());
    }

    //Match and store indexesMemory
    getline(readFile,line);
    db->indexesMemory = new int[db->nIndexes * sizeof(int)];
    for(int i = 0; i < db->nIndexes; i++) {
      getline(readFile,line);
      std::regex_search(line, sm, myRegex);
      db->indexesMemory[i] = std::stoi(sm.str(1).c_str());
    }

    //Match and store configurationGainMatrix
    getline(readFile,line);
    db->configurationGainMatrix = new int*[db->nConfigurations];
    for (int i = 0; i < db->nConfigurations; i++)
      db->configurationGainMatrix[i] = new int[db->nQueries];
    for(int i = 0; i < db->nConfigurations; i++){
      getline(readFile,line);
      int j = 0;
      string::const_iterator searchStart( line.cbegin() );
      while ( std::regex_search( searchStart, line.cend(), sm, myRegex ) ) {
        searchStart = sm.suffix().first;
        db->configurationGainMatrix[i][j] = std::stoi(sm[0]);
        j++;
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
