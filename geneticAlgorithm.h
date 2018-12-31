#ifndef __GENETIC__
#define __GENETIC__

#include <string>
#include <fstream>
#include "database.h"

typedef struct {

	int bestO;
	bool *bestV;

}S_tid;

using namespace std;

class GeneticAlgorithm {

public:
	GeneticAlgorithm(Database *db, int seconds);
	S_tid run();
private:
	Database *instance;	
	bool **population;
	bool *bestSolution;
	bool *parents;		//parents[populationSize]
	bool toPrint;
	int bestObjFunc;
	int populationSize;
	int parentSize;
	int timeout;
	ofstream myfile;
	int fitnessElaboration(bool *vectorToEvaluate);	 //compute the fitness value of the solution
	void populationGeneration(); //fills the **population matrix && fills *fitnessVector
	void solutionSetSelection(); //fills the *parents vector
	void childrenGeneration(int startTime);	 //fills the **children matrix
	bool isFeasibleMemory(bool *vectorToEvaluate);
	bool* getActiveConfig(bool *vectorToEvaluate);
	void storeResult();
	S_tid s_pid;
};

#endif
