#include <iostream>
#include <time.h>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds) {
	this->instance = db;
	this->timeout = seconds;
	this->populationSize = db->nIndexes * 0.001 / 100; //Temporary solution
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
	return;
}
