#include <iostream>
#include <thread>
//#include <pthread.h>
#include "parser.h"
#include "database.h"
#include "geneticAlgorithm.h"

using namespace std;

void doOnThread(Database *db, int seconds, SharedData *shared, string filename) {
	GeneticAlgorithm *algorithm = new GeneticAlgorithm(db, seconds, filename);
	algorithm->run(shared);
	return;
}

int main(int argc, char *argv[]) {
	
	//Checking the n° of arguments.
	if(argc != 4){
		cout << "Invalid arguments\n";
		return 1;
	}

	string instanceName = string(argv[1]);
	//Checking the file extension.
	if(instanceName.find(".odbdp") == string::npos){
		cout << "Need an existing .odbdp file\n";
		return 1;	
	} else {
		ifstream f(argv[1]);
    	if (!f.good()){
    		cout << "File does not exist\n";
    		return 1;		
    	}
	}

	if(string(argv[2]) != "-t"){
		cout << "Usage: -t timelimit\n";
		return 1;
	}

	int seconds;
	try {
 	   seconds = std::stoi (argv[3]);
	} catch (const std::exception& e) {
		cout << "Usage: timelimit must be an integer number\n";
		return 1;
    }
	
	int nThreads = std::thread::hardware_concurrency();
	
	string filename = string(argv[1]).substr (instanceName.size() - 16, 10) + "_OMAMZ_group06.sol";
	std::vector<std::thread> my_array;
	SharedData shared;
	Database *db = new Database();
	Parser *parser = new Parser();
	
	srand(time(0));
    parser->parse(argv[1], db);
	shared.bestSolution = new bool[db->nIndexes];
	shared.bestObjFunc = -1;

	if(nThreads > 5) 
		nThreads = 4;
	cout << "Number of threads: " << nThreads << "\n";
	cout << "Tempo d'inizio: " << time(NULL) << "\n";
    fflush(stdout);
	for (int i = 1; i < nThreads; i++){
		std::thread my_thread(doOnThread, db, seconds, &shared, filename);		
		my_array.push_back(std::move(my_thread));
	}

	doOnThread(db, seconds, &shared, filename);

	for (std::thread & th : my_array){
		if (th.joinable())
			th.join();
	}
	cout << "Tempo di fine : " << time(NULL) << "\n";

    return 0;
}

