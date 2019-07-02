##  Author: Minas Spetsakis
##  Descrn: A short demo program about threads similar to the one in the book
##  Date  :  Sep. 28 2016


SRC = main.c runner.c
OBJ = main.o runner.o
HDR = runner.h

tplay: ${OBJ}
	gcc -pthread ${OBJ} -o tplay

main.o: main.c ${HDR}
	gcc -pthread -c main.c

runner.o: runner.c ${HDR}
	gcc -pthread -c runner.c

clean:
	/bin/rm -f ${OBJ} tplay

tar:
	tar cf tplay.tar ${SRC} ${HDR} makefile
