#include <iostream>
#include <time.h>
#include <math.h>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds) {
	srand(time(0));
	this->instance = db;
	this->timeout = seconds;
	this->populationSize = pow(2,db->nIndexes) * 0.0000000000001; //Temporary solution
	this->population = (bool**) malloc (this->populationSize * sizeof(bool*));
	if (this->population == NULL)
		exit(1);
	for (int i = 0; i < populationSize; i++){
		this->population[i] = (bool*) malloc (db->nIndexes * sizeof(bool));
		if (this->population[i] == NULL)
			exit(1);
	}
	this->children = (bool**) malloc (this->populationSize * sizeof(bool*));
	for (int i = 0; i < populationSize; i++){
		this->population[i] = (bool*) malloc (db->nIndexes * sizeof(bool));
		if (this->population[i] == NULL)
			exit(1);
	}
	this->fitnessVector = (int*) malloc (db->nIndexes * sizeof(int));
}

void GeneticAlgorithm::run() {
	int startTime = time(NULL);
	printf("Tempo d'inizio: %d\n", startTime);
    fflush(stdout);
    populationGeneration();
	while(time(NULL) - startTime < this->timeout) {
		solutionSetSelection();
		childrenGeneration();
		localSearch();
		populationUpdate();
		/*vector to make a try*/
		//bool vectorToEvaluate[50] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
		//fitnessElaboration(vectorToEvaluate);
	}
	printf("Tempo di fine : %ld\n", time(NULL));
}

int GeneticAlgorithm::fitnessElaboration(bool *vectorToEvaluate) {
	int vecSize = instance->nConfigurations;
    bool *vectConfigActive = new bool[vecSize];
    for(int i=0;i<instance->nConfigurations;i++){
        vectConfigActive[i]=1;
    }
    int costInMem=0;

    for(int i=0;i<instance->nIndexes;i++){
       if(vectorToEvaluate[i]==true){costInMem=costInMem+instance->indexesMemory[i];}
	}

	if (costInMem>instance->totalMemory){
		delete [] vectConfigActive;
		cout << "Infeasible\n";
		return -1;
	}
   
    for(int i=0;i<instance->nIndexes;i++){
        if(vectorToEvaluate[i]==0){
            for(int j=0;j<instance->nConfigurations;j++){
                if(vectConfigActive[j]==1){
                    if(instance->configurationIndexMatrix[i][j]==1)
						vectConfigActive[j]=0;
                }
            }
        }
    }

    int totalQueryGain=0;
    for(int i=0;i<instance->nQueries;i++){
		int maxQueryGain=0;
        for(int j=0;j<instance->nConfigurations;j++){
           if(vectConfigActive[j]==1){
			if(instance->configurationGainMatrix[i][j]>maxQueryGain)
                maxQueryGain=instance->configurationGainMatrix[i][j];
          }
        }

		totalQueryGain+=maxQueryGain;
    }

    int buildIndexCost=0;
    for(int i=0;i<instance->nIndexes;i++){
		if(vectorToEvaluate[i]==true){buildIndexCost=buildIndexCost+instance->indexesCost[i];}
	}

	delete [] vectConfigActive;
	cout << totalQueryGain - buildIndexCost;
	cout<<"\n";
	return (totalQueryGain-buildIndexCost);

}

void GeneticAlgorithm::populationGeneration() {
	for(int i = 0; i < this->populationSize; i++)
		for(int j = 0; j < this->instance->nIndexes; j++)
			this->population[i][j] = rand() & 1;
	return;
}

void GeneticAlgorithm::solutionSetSelection() {
	return;
}

void GeneticAlgorithm::childrenGeneration() {
	return;
}

void GeneticAlgorithm::localSearch() {
	return;
}

void GeneticAlgorithm::populationUpdate() {
	bool **tmp = this->population;
	this->population = this->children;
	this->children = tmp;
	for(int i = 0; i < this->populationSize; i++)
		this->fitnessVector[i] = fitnessElaboration(this->population[i]);
	return;
}
