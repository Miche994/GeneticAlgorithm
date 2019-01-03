CFLAGS=-fopenmp -Wpedantic -Wall -D_REENTRANT# -static-libgcc -static-libstdc++
#FOR LINUX
COMPILER=g++
#FOR WINDOWS
#COMPILER=x86_64-w64-mingw32-c++# -static -lpthread

all: 	ODBDPsolver_OMAMZ_group06.exe

ODBDPsolver_OMAMZ_group06.exe: heuristicAlgorithm.o parser.o database.o geneticAlgorithm.o
	$(COMPILER) $(CFLAGS) -o ODBDPsolver_OMAMZ_group06.exe heuristicAlgorithm.o parser.o database.o geneticAlgorithm.o

heuristicAlgorithm.o:	heuristicAlgorithm.cpp parser.h database.h geneticAlgorithm.h
	$(COMPILER) $(CFLAGS) -c heuristicAlgorithm.cpp

geneticAlgorithm.o:	geneticAlgorithm.cpp database.h
	$(COMPILER) $(CFLAGS) -c geneticAlgorithm.cpp

parser.o:	parser.cpp database.h
	$(COMPILER) $(CFLAGS) -c parser.cpp

database.o:	database.cpp database.h
	$(COMPILER) $(CFLAGS) -c database.cpp

.PHONY:	clean

clean:
	rm -f ODBDPsolver_OMAMZ_group06.exe *.o