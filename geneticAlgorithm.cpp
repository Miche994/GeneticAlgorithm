#include <iostream>
#include <time.h>
#include <math.h>
#include <thread>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds, string filename) {
	srand(time(NULL) + std::hash<std::thread::id>{}(std::this_thread::get_id()));	
	this->instance = db;
	this->timeout = seconds;
	this->bestObjFunc = 0;
	this->improved = true;
	this->filename = filename;
	this->lastImprovement = time(NULL);
	this->populationSize = db->nIndexes * 10;
	this->parentSize = this->populationSize * 3 / 4;
	this->population = new bool*[this->populationSize];
	this->parents = new bool[this->populationSize];
	this->bestSolution = new bool[db->nIndexes];
	for (int i = 0; i < populationSize; i++) 
		this->population[i] = new bool[db->nIndexes];
	for (int i = 0; i < this->instance->nIndexes; i++)
		this->bestSolution[i] = false;
}

void GeneticAlgorithm::run(SharedData *shared) {
	int startTime = time(NULL);
    populationGeneration();
	while(time(NULL) - startTime < this->timeout) {
		solutionSetSelection();
		childrenGeneration(startTime);
		if(this->improved == true){
			shared->mut.lock();
			if(this->bestObjFunc > shared->bestObjFunc){
				//Updating shared best solution
				shared->bestObjFunc = this->bestObjFunc;
				for(int i = 0; i < this->instance->nIndexes; i++)
					shared->bestSolution[i] = this->bestSolution[i];
				storeResult();
			}
			this->improved = false;
			shared->mut.unlock();
		}
		if(time(NULL) >= this->lastImprovement + this->instance->nIndexes/4){
			this->bestObjFunc = 0;
			for(int i = 0; i < this->instance->nIndexes; i++)
				this->bestSolution[i] = false;
			srand(time(NULL) + std::hash<std::thread::id>{}(std::this_thread::get_id()) / 3);
			populationGeneration();
			this->lastImprovement = time(NULL);
		}
		
	}
	return;
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
	ofstream myfile;

	//Compute best configs
	for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++){
		bestConfig = -1;
		maxQueryGain = 0;
		for(int j = 0; j < this->instance->nConfigurations; j++){
           	if(vectConfigActive[j] == true){
            	if(this->instance->configurationGainMatrix[j][i] >= maxQueryGain){
                	maxQueryGain = this->instance->configurationGainMatrix[j][i];
                	bestConfig = j;
	    		}
          	}
        }
        configQuery[i] = bestConfig;
	}

	myfile.open(this->filename);
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

  	cout << "Thread n: " << std::this_thread::get_id() << "\n";	
  	cout << "BestObjFunc: " << this->bestObjFunc << "\n";
  	for(int i = 0; i < this->instance->nIndexes; i++)
  		cout << this->bestSolution[i] << " ";
  	cout << "\n\n";
  	fflush(stdout);
	delete [] vectConfigActive;
	delete [] configQuery;
	return;
}

//COMPLETED by computing objective function
int GeneticAlgorithm::fitnessElaboration(bool *vectorToEvaluate) {
	bool *vectConfigActive = this->getActiveConfig(vectorToEvaluate);
	bool *vectConfigWork=new bool[instance->nConfigurations+1];
	int bestconfig=instance->nConfigurations+1;
	int buildIndexCost = 0;
	int totalQueryGain = 0;
    
    for(int j = 0; j < this->instance->nConfigurations+1; j++)
		vectConfigWork[j]=false;

	//Computing query gain (g_cq)
    for(int i = 0, maxQueryGain = 0; i < this->instance->nQueries; i++, maxQueryGain = 0){
        for(int j = 0; j < this->instance->nConfigurations; j++){
           if(vectConfigActive[j] == true){
				if(this->instance->configurationGainMatrix[j][i] >= maxQueryGain){
                maxQueryGain = this->instance->configurationGainMatrix[j][i];
                bestconfig=j;
				}
			}
		}
	    vectConfigWork[bestconfig]=true;
		totalQueryGain += maxQueryGain;
	}

	//Optimizing vectToEvaluate
	for(int i = 0; i < this->instance->nIndexes; i++){
		if(vectorToEvaluate[i] == true){
			int j = 0;
			vectorToEvaluate[i] = false;
			while(j < this->instance->nConfigurations && vectorToEvaluate[i] == false){
				if(vectConfigWork[j] == true && this->instance->configurationIndexMatrix[j][i] == true)
					vectorToEvaluate[i] = true;
				j++;
			}
		}
	}

	//Computing indexes building cost
    for(int i = 0; i < this->instance->nIndexes; i++)
		if(vectorToEvaluate[i] == true)
			buildIndexCost += this->instance->indexesCost[i];

	delete [] vectConfigWork;
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

//COMPLETED
void GeneticAlgorithm::solutionSetSelection() {
	int *popFitness = new int[this->populationSize];
	int currentFitness = 0, sumFitness = 0;
	int selectedParents = 0, currentSum, randomValue, i, counter = 0;

	for(i = 0; i < this->populationSize; i++) {
		currentFitness = fitnessElaboration(this->population[i]);
		sumFitness += abs(currentFitness);
		popFitness[i] = abs(currentFitness); //We could store sqrt
		if(currentFitness > this->bestObjFunc && isFeasibleMemory(this->population[i])){
			improved = true;
			this->lastImprovement = time(NULL);
			this->bestObjFunc = currentFitness;
			for (int j = 0; j < this->instance->nIndexes; j++)
				this->bestSolution[j] = this->population[i][j];	
		}
	}

	for(int i = 0; i < this->populationSize; i++)
		this->parents[i] = false;

	//Roulette Selection
	while(selectedParents < parentSize) {
		if(counter > 100)
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
		} else{
			counter++;
		}
	}
	counter = 0;
	//Tournament Selection 
	while (selectedParents<this->parentSize){
		if(counter > 100)
			break;
		int randomValue1 = rand() % (this->populationSize);
		int randomValue2 = rand() % (this->populationSize);
		int randomValue3 = rand() % (this->populationSize);
		int bestFit = popFitness[randomValue1];
		int bestInd = randomValue1;
		if (bestFit < popFitness[randomValue2]) { bestFit = popFitness[randomValue2]; bestInd = randomValue2; }
		if (bestFit < popFitness[randomValue3]) { bestFit = popFitness[randomValue3]; bestInd = randomValue3; }
		if (this->parents[bestInd] == false){
			selectedParents++;
			this->parents[bestInd] = true;
			counter = 0;
		} else {
			counter++;
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
	int rangeMutation = 70, rangeInversion = 85, maxRange = 100;
	int method = rand() % maxRange;

	if(method <= rangeMutation) method = 0;
	else if (method <= rangeInversion) method = 1;
	else method = 2;

	switch(method) {
		//Mutation
		case 0 : {
			int bitToChange;
			for (int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					bitToChange = rand() % this->instance->nIndexes;
					this->population[i][bitToChange] = !this->population[i][bitToChange];
					bitToChange = rand() % this->instance->nIndexes;
					this->population[i][bitToChange] = !this->population[i][bitToChange];
				} else {
                    for (int j = 0; j < this->instance->nIndexes; j++)
				      this->population[i][j]=this->bestSolution[j] ;
				}
			}
			break;
		}
		//Inversion
		case 1 : {
			int point1, length = this->instance->nIndexes * 0.1;
			bool swapGen;
			do {
				point1 = rand() % this->instance->nIndexes;
			} while (point1 + length < this->instance->nIndexes);
			for(int i = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					for(int k = point1, j = point1 + length; k <= point1 + length; k++, j--){
						swapGen = this->population[i][k];
						this->population[i][k] = this->population[i][j];
						this->population[i][j] = swapGen;
					}
				} else {
                    for (int j = 0; j < this->instance->nIndexes; j++)
				      this->population[i][j]=this->bestSolution[j] ;
				}
			}
			break;
		}
		//Cross
		case 2 : {
			for (int i = 0, j = 0; i < this->populationSize; i++){
				if(this->parents[i] == true){
					for (j = i + 1; j < this->populationSize; j++){
						if(this->parents[j] == true){
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
					if (j >= this->populationSize){
						int bitToChange = rand() % this->instance->nIndexes;
						this->population[i][bitToChange] = !this->population[i][bitToChange];
					}
				} else {
                    for (int j = 0; j < this->instance->nIndexes; j++)
				      this->population[i][j]=this->bestSolution[j] ;
				}
			}
			break;
		}
		default: break;
	}
	return;
}
