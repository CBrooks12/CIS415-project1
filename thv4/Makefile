CFLAGS= -W -Wall

all: thv4

clean:
	rm thv4

thv4: thv4.o p1fxns.o
	cc -o thv4 thv4.o p1fxns.o

thv4.o: thv4.c p1fxns.c p1fxns.h
p1fxns.o: p1fxns.c p1fxns.h
	gcc -g -c -W -Wall p1fxns.c
