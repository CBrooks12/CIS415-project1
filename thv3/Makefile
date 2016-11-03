CFLAGS= -W -Wall

all: thv3

clean:
	rm thv3

thv3: thv3.o p1fxns.o
	cc -o thv3 thv3.o p1fxns.o

thv3.o: thv3.c p1fxns.c p1fxns.h
p1fxns.o: p1fxns.c p1fxns.h
	gcc -g -c -W -Wall p1fxns.c
