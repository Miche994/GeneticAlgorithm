#include <iostream>
#include <time.h>
#include <math.h>
#include <thread>
#include "geneticAlgorithm.h"

using namespace std;

GeneticAlgorithm::GeneticAlgorithm(Database *db, int seconds, string filename) {
	srand(time(0));
	this->instance = db;
	this->timeout = seconds;
	this->populationSize = db->nIndexes * 10;
	this->parentSize = this->populationSize * 7 / 8;
	this->population = new bool*[this->populationSize];
	for (int i = 0; i < populationSize; i++) 
		this->population[i] = new bool[db->nIndexes];
	this->parents = new bool[this->populationSize];
	this->bestSolution = new bool[db->nIndexes];
	this->bestObjFunc = 0;
	this->toPrint = true;
	this->filename = filename;
}

void GeneticAlgorithm::run(SharedData *shared) {
	int startTime = time(NULL);
    populationGeneration();
	while(time(NULL) - startTime < this->timeout) {
		solutionSetSelection();
		childrenGeneration(startTime);
		if(this->toPrint == true){
			shared->mut.lock();
			if(this->bestObjFunc > shared->bestObjFunc){
				//Updating shared best solution
				shared->bestObjFunc = this->bestObjFunc;
				for(int i = 0; i < this->instance->nIndexes; i++)
					shared->bestSolution[i] = this->bestSolution[i];
				storeResult();
			}
			this->toPrint = false;
			shared->mut.unlock();
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

	ofstream myfile;
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

  	cout << "\nThread n: " << std::this_thread::get_id() << "\n";	
  	cout << "BestObjFunc: " << this->bestObjFunc << "\n";
  	for(int i = 0; i < this->instance->nIndexes; i++)
  		cout << this->bestSolution[i] << " ";
  	cout << "\n\n";
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
		if(counter++ > 20)
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
	int rangeMutation = 30, rangeInversion = 50, maxRange = 100;
	int method = rand() % maxRange;
	if(time(NULL) / (startTime + this->timeout) < 0.8){
		rangeMutation = 70;
		rangeInversion = 90;
	}

	if(method <= rangeMutation) method = 0;
	else if (method <= rangeInversion) method = 1;
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
