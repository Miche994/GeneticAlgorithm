all: 	heuristicAlgorithm

heuristicAlgorithm: heuristicAlgorithm.cpp
	g++ heuristicAlgorithm.cpp parser.cpp database.cpp -ansi -Wpedantic -Wall -D_REENTRANT -o heuristicAlgorithm

.PHONY:	clean

clean:
	rm -f heuristicAlgorithm
