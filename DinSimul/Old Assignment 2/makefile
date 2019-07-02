# Author: Minas Spetsakis
# Date: Nov. 10 2016
# Description: Solution to Assignment II (Threaded simulator)

simul: simul.o queue.o error.o
	gcc simul.o queue.o error.o -pthread -lc -o simul

simul.o: simul.c args.h queue.h error.h
	gcc -c -pthread simul.c

queue.o: queue.c queue.h error.h
	gcc -c -pthread queue.c

error.o: error.c error.h
	gcc -c error.c

clean:
	/bin/rm simul.o queue.o error.o simul

tar:
	tar cf Simul.tar *.c *.h makefile Marking

