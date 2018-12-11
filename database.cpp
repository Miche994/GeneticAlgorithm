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
		   		"@@|->indexesCost: \n\t";
	for(int i=0; i<this->nIndexes*this->nConfigurations; i++){
		s += std::to_string(this->configurationIndexMatrix[i]) + " ";
		if((i+1)%this->nIndexes == 0)
			s += "\n\t";
	}
	s += "\n";
	return s;
}
