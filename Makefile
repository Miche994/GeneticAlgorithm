all: 	heuristicAlgorithm.exe

heuristicAlgorithm.exe: heuristicAlgorithm.cpp
	g++ heuristicAlgorithm.cpp parser.cpp database.cpp -Wpedantic -Wall -D_REENTRANT -o heuristicAlgorithm.exe

.PHONY:	clean

clean:
	rm -f heuristicAlgorithm.exe
