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
    db->configurationIndexMatrix = (bool*) malloc (db->nIndexes * db->nConfigurations * sizeof(bool));
    if (db->configurationIndexMatrix == NULL) 
      exit (1);
    for(int i = 0; i < db->nConfigurations; i++){
      getline(readFile,line);
      int index = i * db->nIndexes;
      string::const_iterator searchStart( line.cbegin() );
      while ( std::regex_search( searchStart, line.cend(), sm, myRegex ) ) {
        searchStart = sm.suffix().first;
        if(sm[0] == "1")
          db->configurationIndexMatrix[index] = true;
        else
          db->configurationIndexMatrix[index] = false;
        index++;
      }
    }
    
    //Match and store indexesCost
    getline(readFile,line);
    db->indexesCost = (int*) malloc (db->nIndexes * sizeof(int));
    if (db->indexesCost == NULL) 
      exit (1);
    for(int i = 0; i < db->nIndexes; i++) {
      getline(readFile,line);
      std::regex_search(line, sm, myRegex);
      db->indexesCost[i] = std::stoi(sm.str(1).c_str());
    }

    //Match and store indexesMemory
    getline(readFile,line);
    db->indexesMemory = (int*) malloc (db->nIndexes * sizeof(int));
    if (db->indexesMemory == NULL) 
      exit (1);
    for(int i = 0; i < db->nIndexes; i++) {
      getline(readFile,line);
      std::regex_search(line, sm, myRegex);
      db->indexesMemory[i] = std::stoi(sm.str(1).c_str());
    }

    //Match and store configurationGainMatrix
    getline(readFile,line);
    db->configurationGainMatrix = (int*) malloc (db->nIndexes * db->nConfigurations * sizeof(int));
    if (db->configurationGainMatrix == NULL) 
      exit (1);
    for(int i = 0; i < db->nConfigurations; i++){
      getline(readFile,line);
      int index = i * db->nIndexes;
      string::const_iterator searchStart( line.cbegin() );
      while ( std::regex_search( searchStart, line.cend(), sm, myRegex ) ) {
        searchStart = sm.suffix().first;
        db->configurationGainMatrix[index] = std::stoi(sm[0]);
        index++;
      }
    }
    //cout << db->toString();
    readFile.close();
  }
  else 
  {
    cout << "File doesn't exist or doesn't have read privilege\n";
  }
  return;
}
