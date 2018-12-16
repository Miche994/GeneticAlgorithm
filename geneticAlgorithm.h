#ifndef __GENETIC__
#define __GENETIC__

#include <string>
#include "database.h"

using namespace std;

class GeneticAlgorithm {

public:
	GeneticAlgorithm(Database *db, int seconds);
	void run();
private:
	int timeout;
	Database *instance;
};

#endif
