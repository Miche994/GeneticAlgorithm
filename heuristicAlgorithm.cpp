#include <iostream>
#include <thread>
#include "parser.h"
#include "database.h"
#include "geneticAlgorithm.h"



using namespace std;

// La funzione "doOnThread()" inizialmente era unica. E' stata triplicata solo per fare dei test.

S_tid doOnThread_1(Database *db, int seconds) {

	std::chrono::milliseconds timespan(2000); // Inserite solo per fare delle prove. Commentare se non servono 

	std::this_thread::sleep_for(timespan); //

	cout << "\n\n I am Thread " << std::this_thread::get_id << "\n\n";

	GeneticAlgorithm *algorithm_1 = new GeneticAlgorithm(db, seconds);
	return algorithm_1->run();

}
S_tid doOnThread_2(Database *db, int seconds) {

	std::chrono::milliseconds timespan(4000); // Inserite solo per fare delle prove. Commentare se non servono 

	std::this_thread::sleep_for(timespan);

	cout << "\n\n I am Thread " << std::this_thread::get_id << "\n\n";

	GeneticAlgorithm *algorithm_2 = new GeneticAlgorithm(db, seconds);
	return algorithm_2->run();

}
S_tid doOnThread_3(Database *db, int seconds) {

	std::chrono::milliseconds timespan(6000); // Inserite solo per fare delle prove. Commentare se non servono 

	std::this_thread::sleep_for(timespan);

	cout << "\n\n I am Thread " << std::this_thread::get_id << "\n\n";

	GeneticAlgorithm *algorithm_3 = new GeneticAlgorithm(db, seconds);
	return algorithm_3->run();

}

int main(int argc, char *argv[]) {
	
	//Checking the n° of arguments.
	if(argc != 3){
		cout << "Invalid arguments\n";
		return 1;
	}

	//Checking the file extension.
	if(string(argv[1]).find(".odbdp") == string::npos){
		cout << "Need an .odbdp file\n";
		return 1;	
	}
	
	int seconds = std::stoi (argv[2]);
	Database *db = new Database();
	Parser *parser = new Parser();


    parser->parse(argv[1], db);

	// La creazione e la distruzione dei Thread viene eseguita correttamente.
	////////////////

	//Create Thread
	std::thread first(doOnThread_1, db, seconds);
	std::thread second(doOnThread_2, db, seconds);
	std::thread third(doOnThread_3, db, seconds);
	
	//Wait Thread
	first.join();
	second.join();
	third.join();
	
	////////////////

    return 0;
}

