#include <iostream>
#include <fstream>
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
	if(this->populationSize < 4)
		this->populationSize = pow(2,db->nIndexes);
	this->parentSize = this->populationSize * 3 / 4;
	this->population = new bool*[this->populationSize];
	for (int i = 0; i < populationSize; i++) 
		this->population[i] = new bool[db->nIndexes];
	this->parents = new bool[this->populationSize];
	this->fitnessVector = new int[db->nIndexes];
	this->bestSolution = new bool[db->nIndexes];
	this->bestObjFunc = 0;
	this->toPrint = true;
}

void GeneticAlgorithm::run() {
	int startTime = time(NULL);
	cout << "Tempo d'inizio: " << startTime << "\n";
    fflush(stdout);
    populationGeneration();
	while(time(NULL) - startTime < this->timeout) {
		solutionSetSelection();
		childrenGeneration();
		if(this->toPrint == true)
			storeResult();
	}
	cout << "Tempo di fine : " << time(NULL) << "\n";
}

bool GeneticAlgorithm::isFeasibleMemory(bool *vectorToEvaluate){
	int costInMem = 0;

    for(int i = 0; i < this->instance->nIndexes; i++)
		if(vectorToEvaluate[i] == true)
       		costInMem += this->instance->indexesMemory[i];
	if (costInMem > this->instance->totalMemory)
		return false;
	return true;
}

bool* GeneticAlgorithm::getActiveConfig(bool *vectorToEvaluate){
	bool *vectorConfigActive = new bool[this->instance->nConfigurations];
	for(int i = 0; i < this->instance->nConfigurations; i++)
        vectorConfigActive[i] = 1;

	for(int i = 0; i < this->instance->nIndexes; i++){
        if(vectorToEvaluate[i] == 0){
            for(int j = 0; j < this->instance->nConfigurations; j++){
                if(vectorConfigActive[j] == 1){
                    if(this->instance->configurationIndexMatrix[j][i] == 1)
						vectorConfigActive[j] = 0;
                }
            }
        }
    }
	return vectorConfigActive;
}

void GeneticAlgorithm::storeResult(){
	bool *vectConfigActive = this->getActiveConfig(this->bestSolution);
	int *configQuery = new int[this->instance->nQueries];
	int bestConfig;

	//Compute best configs
	for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++){
		bestConfig = 0;
		maxQueryGain = 0;
		for(int j = 0; j < this->instance->nConfigurations; j++){
           	if(vectConfigActive[j] == 1){
            	if(this->instance->configurationGainMatrix[j][i] > maxQueryGain){
                	maxQueryGain = this->instance->configurationGainMatrix[j][i];
                	bestConfig = j;
	    		}
          	}
        }
        configQuery[i] = bestConfig;
	}

	ofstream myfile;
  	myfile.open ("output.sol");
	//Storing result on file
	for(int i = 0; i < this->instance->nConfigurations; i++){
		for(int j = 0; j < this->instance->nQueries; j++)
			if(i == configQuery[j])
				myfile << "1 ";
			else
				myfile << "0 ";
		myfile << "\n";
	}

  	myfile.close();

  	cout << "BestObjFunc: " << this->bestObjFunc << "\n";
  	for(int i = 0; i < this->instance->nIndexes; i++)
  		cout << this->bestSolution[i] << " ";
  	cout << "\n";
  	this->toPrint = false;
	delete [] vectConfigActive;
	delete [] configQuery;
	return;
}

//COMPLETED by computing objective function
int GeneticAlgorithm::fitnessElaboration(bool *vectorToEvaluate) {
	bool *vectConfigActive = this->getActiveConfig(vectorToEvaluate);
	int buildIndexCost = 0;
	int totalQueryGain = 0;

	//Computing query gain (g_cq)
    for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++, maxQueryGain = 0){
        for(int j = 0; j < this->instance->nConfigurations; j++){
           if(vectConfigActive[j] == 1){
			if(this->instance->configurationGainMatrix[j][i] >= maxQueryGain)
                maxQueryGain = this->instance->configurationGainMatrix[j][i];
          }
        }
		totalQueryGain += maxQueryGain;
    }

	//Optimizing vectToEvaluate
	for(int i = 0; i < this->instance->nIndexes; i++){
		if(vectorToEvaluate[i] == 1){
			int j = 0;
			vectorToEvaluate[i] = 0;
			while(j < this->instance->nConfigurations && vectorToEvaluate[i] == 0){
				if(vectConfigActive[j] == 1 && this->instance->configurationIndexMatrix[j][i] == 1)
					vectorToEvaluate[i] = 1;
				j++;
			}
		}
	}

	//Computing indexes building cost
    for(int i = 0; i < this->instance->nIndexes; i++)
		if(vectorToEvaluate[i] == true)
			buildIndexCost += this->instance->indexesCost[i];

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
	int currentFitness = 0, sumFitness = 0;
	int selectedParents = 0, currentSum, randomValue, i, counter = 0;

	for(i = 0; i < this->populationSize; i++) {
		currentFitness = fitnessElaboration(this->population[i]);
		sumFitness += currentFitness;
		popFitness[i] = currentFitness; //We could store sqrt
		if(currentFitness > this->bestObjFunc && isFeasibleMemory(this->population[i])){
			toPrint = true;
			this->bestObjFunc = currentFitness;
			for (int j = 0; j < this->instance->nIndexes; j++)
				this->bestSolution[j] = this->population[i][j];	
		}
	}

	for(int i = 0; i < this->populationSize; i++)
		this->parents[i] = false;

	//Roulette untill it fails 15 times sequentially
	while(selectedParents < parentSize) {
		if(counter++ > 15) 
			break;
		i = 0;
		currentSum = popFitness[0];
		randomValue = rand() % (sumFitness+1);
		while(currentSum < randomValue && currentSum <= sumFitness && i < this->populationSize) {
			i++;
			currentSum += popFitness[i];
		}
		if(currentSum <= sumFitness && this->parents[i] == false){
			this->parents[i] = true;
			selectedParents += 1;
			counter = 0;
		}
	}
	//Selection from a random point of the remaining parents
	randomValue = rand()%(this->populationSize-this->parentSize);
	while(selectedParents < this->parentSize) {
		if(this->parents[randomValue] == false){
			this->parents[randomValue] = true;
			selectedParents++;
		}
		if(randomValue++ == this->populationSize)
			randomValue = 0;
	}

	delete [] popFitness;
	return;
}

void GeneticAlgorithm::childrenGeneration() {
	int method = rand() % 2;
	switch(method) {
		//Mutation
		case 0 : {
			int bitToChange;
			for (int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					bitToChange = rand() % this->instance->nIndexes;
					this->population[i][bitToChange] = !this->population[i][bitToChange];
				}
			}
			break;
		}
		/*Cross
		case 1 : {
			break;
		}*/
		//Inversion
		case 1 : {
			int point1, point2, swapPt;
			bool swapGen;
			do {
				point1 = rand() % this->instance->nIndexes;
				point2 = rand() % this->instance->nIndexes; 
			} while (point1 == point2 || abs(point1-point2) < 2);
			if(point1 > point2){
				swapPt = point1;
				point1 = point2;
				point2 = swapPt;
			}
			//Updating
			for(int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					for(int k = 0; point1 + k < point2 - k; k++){
						swapGen = this->population[i][k + point1];
						this->population[i][point1 + k] = this->population[i][point2 - k];
						this->population[i][point2 - k] = swapGen;
					}
				}
			}
			break;
		}
		default: break;
	}
	return;
}

//TODO@@@@@@ if we want improve children before pushing them in the population
void GeneticAlgorithm::localSearch() {
	return;
}
