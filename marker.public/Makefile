SRC = main.c fork.c
OBJ = main.o fork.o
HDR = fork.h

TRG = marker

${TRG}: ${HDR} ${OBJ}
	gcc ${OBJ} -o ${TRG}

main.o: main.c ${HDR}
	gcc -c main.c -o main.o

fork.o: fork.c ${HDR}
	gcc -c fork.c -o fork.o

clean:
	/bin/rm -f ${OBJ} ${TRG} test.out test.err *~

