all: 	heuristicAlgorithm

heuristicAlgorithm: heuristicAlgorithm.cpp
	g++ heuristicAlgorithm.cpp parser.cpp -ansi -Wpedantic -Wall -D_REENTRANT -o heuristicAlgorithm

.PHONY:	clean

clean:
	rm -f heuristicAlgorithm
