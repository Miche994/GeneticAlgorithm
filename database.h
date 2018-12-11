#ifndef __DATABASE__
#define __DATABASE__

#include <string>

using namespace std;

class Database {

public:
	Database();
	string toString();
	int nQueries;
	int nIndexes;
	int nConfigurations;
	int totalMemory;
	bool *configurationIndexMatrix;
	int *indexesCost;
	int *indexesMemory;
	bool *configurationGainMatrix;
};

#endif
