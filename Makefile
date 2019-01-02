CFLAGS=-fopenmp -Wpedantic -Wall -D_REENTRANT

all: 	ODBDPsolver_OMAMZ_group06.exe

ODBDPsolver_OMAMZ_group06.exe: heuristicAlgorithm.o parser.o database.o geneticAlgorithm.o
	g++ $(CFLAGS) -o ODBDPsolver_OMAMZ_group06.exe heuristicAlgorithm.o parser.o database.o geneticAlgorithm.o

heuristicAlgorithm.o:	heuristicAlgorithm.cpp parser.h database.h geneticAlgorithm.h
	g++ $(CFLAGS) -c heuristicAlgorithm.cpp

geneticAlgorithm.o:	geneticAlgorithm.cpp database.h
	g++ $(CFLAGS) -c geneticAlgorithm.cpp

parser.o:	parser.cpp database.h
	g++ $(CFLAGS) -c parser.cpp

database.o:	database.cpp database.h
	g++ $(CFLAGS) -c database.cpp

.PHONY:	clean

clean:
	rm -f ODBDPsolver_OMAMZ_group06.exe *.o