# Author: Minas Spetsakis
# Date: Nov. 10 2016
# Description: Solution to Assignment II (Threaded simulator)

SRC = simul.c error.c
OBJ = simul.o error.o
HDR = args.h error.h

TRG = simul

${TRG}: ${OBJ}
	gcc ${OBJ} -pthread -lc -o ${TRG}

simul.o: simul.c ${HDR}
	gcc -c -pthread simul.c

error.o: error.c ${HDR}
	gcc -c error.c

clean:
	/bin/rm ${OBJ} ${TRG}

tar:
	tar cf DinSimul.tar ${SRC} ${HDR} Makefile
	../sendit DinSimul.tar

