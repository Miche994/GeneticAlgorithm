#include <iostream>
#include <time.h>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds) {
	this->instance = db;
	this->timeout = seconds;
}

void GeneticAlgorithm::run() {
	int startTime = time(NULL);
	printf("Tempo d'inizio: %d\n", startTime);
    fflush(stdout);
	while(time(NULL) - startTime < this->timeout) {

	}
	printf("Tempo di fine : %ld\n", time(NULL));
}
