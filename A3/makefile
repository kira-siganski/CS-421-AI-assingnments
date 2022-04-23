All: assign3.o

CSPgen.o: CSPgen.cpp CSPgen.h Structures.h
	g++ -c CSPgen.cpp

ArcConsistency.o: ArcConsistency.cpp ArcConsistency.h Structures.h 
	g++ -c ArcConsistency.cpp

Backtracking.o: Backtracking.cpp Backtracking.h Structures.h
	g++ -c Backtracking.cpp

ForwardChecking.o:ForwardChecking.cpp ForwardChecking.h Backtracking.h Structures.h
	g++ -c ForwardChecking.cpp

FLA.o: FLA.cpp FLA.h Structures.h Backtracking.h ArcConsistency.h
	g++ -c FLA.cpp

Main.o: Main.cpp CSPgen.h Structures.h ArcConsistency.h Backtracking.h ForwardChecking.h FLA.h
	g++ -c Main.cpp

assign3.o: CSPgen.o Main.o ArcConsistency.o Backtracking.o ForwardChecking.o FLA.o
	g++ CSPgen.o ArcConsistency.o Backtracking.o ForwardChecking.o FLA.o Main.o -o assign3

clean: 
	rm -r *.o