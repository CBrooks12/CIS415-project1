CFLAGS= -W -Wall

all: thv2

clean:
	rm thv2

thv2: thv2.o p1fxns.o
	cc -o thv2 thv2.o p1fxns.o

thv2.o: thv2.c p1fxns.c p1fxns.h
p1fxns.o: p1fxns.c p1fxns.h
	gcc -g -c -W -Wall p1fxns.c
