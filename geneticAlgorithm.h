#ifndef __GENETIC__
#define __GENETIC__

#include <string>
#include <fstream>
#include <mutex>
#include "database.h"

using namespace std;

typedef struct {
	std::mutex mut;
	bool *bestSolution;
	int bestObjFunc;

}SharedData;

class GeneticAlgorithm {

public:
	GeneticAlgorithm(Database *db, int seconds, string filename);
	void run(SharedData *shared);
private:
	Database *instance;	
	bool **population;
	bool *bestSolution;
	bool *parents;		//parents[populationSize]
	bool improved;
	int bestObjFunc;
	int populationSize;
	int parentSize;
	int timeout;
	int lastImprovement;
	string filename;
	int fitnessElaboration(bool *vectorToEvaluate);	 //compute the fitness value of the solution
	void populationGeneration(); //fills the **population matrix && fills *fitnessVector
	void solutionSetSelection(); //fills the *parents vector
	void childrenGeneration(int startTime);	 //fills the **children matrix
	bool isFeasibleMemory(bool *vectorToEvaluate);
	bool* getActiveConfig(bool *vectorToEvaluate);
	void storeResult();
};

#endif
