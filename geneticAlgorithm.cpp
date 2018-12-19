#include <iostream>
#include <time.h>
#include <math.h>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds) {
	srand(time(0));
	this->instance = db;
	this->timeout = seconds;
	//3.85 is the best fit to make the popSize increase considering all the solutionSet
	this->populationSize = pow(2,db->nIndexes) * pow(10,-db->nIndexes/3.85); 
	this->parentSize = this->populationSize * 3 / 4;
	this->population = new bool*[this->populationSize];
	this->children = new bool*[this->parentSize];
	for (int i = 0; i < populationSize; i++) {
		this->population[i] = new bool[db->nIndexes];
		this->children[i] = new bool[db->nIndexes];
	}
	this->parents = new bool[this->populationSize] {false};
	this->fitnessVector = new int[db->nIndexes];
	this->bestSolution = new bool[db->nIndexes];
	this->bestObjFunc = 0;
}

void GeneticAlgorithm::run() {
	int startTime = time(NULL);
	cout << "Tempo d'inizio: " << startTime << "\n";
    fflush(stdout);
    populationGeneration();
	while(time(NULL) - startTime < this->timeout) {
		solutionSetSelection();
		childrenGeneration();
		//localSearch();
		populationUpdate();
		cout << "Best Objective function: " << this->bestObjFunc << "\n";
		cout << "Best solution:\n";
		for(int i = 0; i<this->instance->nIndexes;i++)
			cout << this->bestSolution[i] << " ";
		cout << "\n";
	}
	cout << "Tempo di fine : " << time(NULL) << "\n";
}

bool GeneticAlgorithm::isFeasibleMemory(bool *vectorToEvaluate){
	int costInMem=0;

    for(int i = 0; i < this->instance->nIndexes; i++)
		if(vectorToEvaluate[i] == true)
       		costInMem=costInMem+this->instance->indexesMemory[i];
	if (costInMem>this->instance->totalMemory)
		return false;
	return true;
}

//COMPLETED by computing objective function
int GeneticAlgorithm::fitnessElaboration(bool *vectorToEvaluate) {
	int vecSize = instance->nConfigurations;
    bool *vectConfigActive = new bool[vecSize];
    for(int i=0;i<instance->nConfigurations;i++)
        vectConfigActive[i]=1;
   
    for(int i=0;i<instance->nIndexes;i++){
        if(vectorToEvaluate[i]==0){
            for(int j=0;j<instance->nConfigurations;j++){
                if(vectConfigActive[j]==1){
                    if(instance->configurationIndexMatrix[j][i]==1)
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
			if(instance->configurationGainMatrix[j][i]>maxQueryGain)
                maxQueryGain=instance->configurationGainMatrix[j][i];
          }
        }

		totalQueryGain+=maxQueryGain;
    }

    int buildIndexCost=0;
    for(int i=0;i<instance->nIndexes;i++){
		if(vectorToEvaluate[i]==true){buildIndexCost=buildIndexCost+instance->indexesCost[i];}
	}

	delete [] vectConfigActive;
	return (totalQueryGain-buildIndexCost);
}

//COMPLETED with random generation
void GeneticAlgorithm::populationGeneration() {
	for(int i = 0; i < this->populationSize; i++)
		for(int j = 0; j < this->instance->nIndexes; j++)
			this->population[i][j] = rand() & 1;
	return;
}

//COMPLETED with Roulette Wheel Selection method
void GeneticAlgorithm::solutionSetSelection() {
	int *popFitness = new int[this->populationSize];
	int currentFitness = 0;
	int sumFitness = 0;
	for(int i = 0; i < this->populationSize; i++) {
		currentFitness = fitnessElaboration(this->population[i]);
		sumFitness += currentFitness;
		popFitness[i] = currentFitness; //We could store sqrt
		if(currentFitness > this->bestObjFunc && isFeasibleMemory(this->population[i])){
			this->bestObjFunc = currentFitness;
			this->bestSolution = this->population[i];
		}
	}

	for(int i = 0; i < this->populationSize; i++)
		this->parents[i] = false;

	//to implement "Random" parents selection
	int selectedParents = 0;
	int randomValue; int currentSum; int i;
	while(selectedParents < parentSize) {
		i = 0;
		currentSum = popFitness[0];
		randomValue = rand() % sumFitness;
		while(currentSum < randomValue && currentSum <= sumFitness) {
			i++;
			currentSum += popFitness[i];
		}
		if(currentSum <= sumFitness && this->parents[i] == false){
			this->parents[i] = true;
			selectedParents += 1;
		}
	}

	delete [] popFitness;
	return;
}

void GeneticAlgorithm::childrenGeneration() {
	for (int i = 0, j = 0; i < this->populationSize; i++){
		if(this->parents[i] == true){
			int bitToChange = rand() % this->instance->nIndexes;
			this->children[j] = this->population[i];			
			this->children[j][bitToChange] = !this->children[j][bitToChange];
			j++;
		}
	}
	return;
}

//TODO@@@@@@ if we want improve children before pushing them in the population
void GeneticAlgorithm::localSearch() {
	return;
}

//COMPLETED by switching parents with children 
void GeneticAlgorithm::populationUpdate() {
	/*
	bool **tmp = this->population;
	this->population = this->children;
	this->children = tmp;
	*/
	bool *tmp ;
	for(int i = 0, j = 0; i < this->populationSize; i++)
		if(this->parents[i] == true){
			tmp = this->population[i];
			this->population[i] = this->children[j];
			this->children[j] = tmp;
			j++;
		}
	return;
}
