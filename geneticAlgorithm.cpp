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
	//this->populationSize = pow(2,db->nIndexes) * pow(10,-db->nIndexes/3.85) * 300; 	
	this->populationSize = db->nIndexes * 300;
	this->parentSize = this->populationSize * 3 / 4;
	this->population = new bool*[this->populationSize];
	for (int i = 0; i < populationSize; i++) 
		this->population[i] = new bool[db->nIndexes];
	this->parents = new bool[this->populationSize];
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
		childrenGeneration(startTime);
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
        vectorConfigActive[i] = true;

	for(int i = 0; i < this->instance->nIndexes; i++){
        if(vectorToEvaluate[i] == false){
            for(int j = 0; j < this->instance->nConfigurations; j++){
                if(vectorConfigActive[j] == true){
                    if(this->instance->configurationIndexMatrix[j][i] == true)
						vectorConfigActive[j] = false;
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
		bestConfig = -1;
		maxQueryGain = 0;
		for(int j = 0; j < this->instance->nConfigurations; j++){
           	if(vectConfigActive[j] == 1){
            	if(this->instance->configurationGainMatrix[j][i] >= maxQueryGain){
                	maxQueryGain = this->instance->configurationGainMatrix[j][i];
                	bestConfig = j;
	    		}
          	}
        }
        configQuery[i] = bestConfig;
	}

  	this->myfile.open ("output.sol");
	//Storing result on file
	for(int i = 0; i < this->instance->nConfigurations; i++){
		for(int j = 0; j < this->instance->nQueries; j++)
			if(i == configQuery[j])
				this->myfile << "1 ";
			else
				this->myfile << "0 ";
		this->myfile << "\n";
	}
  	this->myfile.close();

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
    /*for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++){
    	maxQueryGain = 0;
        for(int j = 0; j < this->instance->nConfigurations; j++){
           if(vectConfigActive[j] == true){
			if(this->instance->configurationGainMatrix[j][i] > maxQueryGain)
                maxQueryGain = this->instance->configurationGainMatrix[j][i];
          }
        }
		totalQueryGain += maxQueryGain;
    }*/

    bool *vectConfigwork=new bool[instance->nConfigurations+1];
    for(int j = 0; j < this->instance->nConfigurations+1; j++)
		vectConfigwork[j]=false;
    int bestconfig=instance->nConfigurations+1;
	//Computing query gain (g_cq)
    for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++, maxQueryGain = 0){
        for(int j = 0; j < this->instance->nConfigurations; j++){
           if(vectConfigActive[j] == 1){
				if(this->instance->configurationGainMatrix[j][i] >= maxQueryGain){///////////////////////
                maxQueryGain = this->instance->configurationGainMatrix[j][i];
                bestconfig=j;
			}
		}
	}
	    vectConfigwork[bestconfig]=true;
		totalQueryGain += maxQueryGain;
	}

	//Optimizing vectToEvaluate
	for(int i = 0; i < this->instance->nIndexes; i++){
		if(vectorToEvaluate[i] == true){
			int j = 0;
			vectorToEvaluate[i] = false;
			while(j < this->instance->nConfigurations && vectorToEvaluate[i] == false){
				if(vectConfigwork[j] == true && this->instance->configurationIndexMatrix[j][i] == true)
					vectorToEvaluate[i] = true;
				j++;
			}
		}
	}

	//Computing indexes building cost
    for(int i = 0; i < this->instance->nIndexes; i++)
		if(vectorToEvaluate[i] == true)
			buildIndexCost += this->instance->indexesCost[i];

	delete [] vectConfigwork;
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
		if(counter++ > 30)
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

void GeneticAlgorithm::childrenGeneration(int startTime) {
	int rangeMutation = 2, rangeCross = 6, maxRange = 10;
	if(time(NULL) / (startTime + this->timeout) < 0.5){
		rangeMutation = 8;
		rangeCross = 9;
	}
	int method = rand() % maxRange;
	if(method <= rangeMutation) method = 0;
	else if (method <= rangeCross) method = 1;
	else method = 2;

	switch(method) {
		//Mutation
		case 0 : {
			int bitToChange1, bitToChange2;
			for (int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					bitToChange1 = rand() % this->instance->nIndexes;
					bitToChange2 = rand() % this->instance->nIndexes;
					this->population[i][bitToChange1] = !this->population[i][bitToChange1];
					this->population[i][bitToChange2] = !this->population[i][bitToChange2];
				}
			}
			break;
		}
		//Cross
		case 1 : {
			for (int i = 0, j = 0; i < this->populationSize; i++){
				//finding the 1st parent
				if(this->parents[i] == true){
					for (j = i + 1; j < this->populationSize; j++){
						//finding the 2nd one
						if(this->parents[j] == true){
							//Swapping
							bool tmp;
							for(int k = rand() % this->instance->nIndexes; k < this->instance->nIndexes; k++){
								tmp = this->population[i][k];
								this->population[i][k] = this->population[j][k];
								this->population[j][k] = tmp;
							}
							i = j + 1;
							break;
						}
					}
					//there's an odd parent
					if (j >= this->populationSize){
						int bitToChange = rand() % this->instance->nIndexes;
						this->population[i][bitToChange] = !this->population[i][bitToChange];
					}
				}
			}
			break;
		}
		//Inversion
		case 2 : {
			int point1, length = this->instance->nIndexes * 0.1;
			bool swapGen;
			do {
				point1 = rand() % this->instance->nIndexes;
			} while (point1 + length < this->instance->nIndexes);
			//Updating
			for(int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					for(int k = point1, j = point1 + length; k <= point1 + length; k++, j--){
						swapGen = this->population[i][k];
						this->population[i][k] = this->population[i][j];
						this->population[i][j] = swapGen;
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
