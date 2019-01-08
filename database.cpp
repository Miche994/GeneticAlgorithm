#include <iostream>
#include "database.h"

using namespace std;

Database::Database() {
}

string Database::toString() {
	string s =  "@@Database class printed!\n" 
		   		"@@|->nQueries: " + std::to_string(this->nQueries) + "\n"
		   		"@@|->nIndexes: " + std::to_string(this->nIndexes) + "\n"
		   		"@@|->nConfigurations: " + std::to_string(this->nConfigurations) + "\n"
		   		"@@|->totalMemory: " + std::to_string(this->totalMemory) + "\n"
		   		"@@|->configurationIndexMatrix: \n\t";
	for(int i=0; i<this->nConfigurations; i++){
		for(int j = 0; j < this->nIndexes; j++)
			s += std::to_string(this->configurationIndexMatrix[i][j]) + " ";
		s += "\n\t";
	}
	
	s += "\n@@|->indexesCost: \n\t";
	for(int i = 0; i < this->nIndexes; i++) {
		s += std::to_string(this->indexesCost[i]) + " ";
		if((i+1) == this->nIndexes/2)
			s += "\n\t";
	}
	
	s += "\n@@|->indexesMemory: \n\t";
	for(int i = 0; i < this->nIndexes; i++) {
		s += std::to_string(this->indexesMemory[i]) + " ";
		if((i+1) == this->nIndexes/2)
			s += "\n\t";
	}
	
	s += "\n@@|->configurationGainMatrix: \n\t";
	for(int i=0; i<this->nConfigurations; i++){
		for (int j = 0; j < this->nQueries; j++)
			s += std::to_string(this->configurationGainMatrix[i][j]) + " ";
		s += "\n\t";
	}
	s += "\n";
	return s;
}
