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
	}
	printf("Tempo di fine : %ld\n", time(NULL));
}

int GeneticAlgorithm::fitnessElaboration(bool *vectorToEvaluate) {
	return 0;
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
