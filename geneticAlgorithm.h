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
	Database *instance;	
	bool **children;
	bool **population;
	bool *bestSolution;
	bool *parents;		//parents[populationSize]
	int *fitnessVector;
	int bestObjFunc;
	int populationSize;
	int timeout;
	int fitnessElaboration(bool *vectorToEvaluate);	 //compute the fitness value of the solution
	void populationGeneration(); //fills the **population matrix && fills *fitnessVector
	void solutionSetSelection(); //fills the *parents vector
	void childrenGeneration();	 //fills the **children matrix
	void localSearch();			 //improves **children and call fitnessElaboration() of new solutions
	void populationUpdate();	 //updates **population by taking the best children && fills (updates) *fitnessVector
};

#endif
