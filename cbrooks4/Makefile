CFLAGS= -W -Wall

all: thv1
all: thv2
all: thv3
all: thv4

clean:
	rm thv1
	rm thv2		
	rm thv3
	rm thv4
	rm *.o
	
thv1: thv1.o p1fxns.o
	cc -o thv1 thv1.o p1fxns.o

thv2: thv2.o p1fxns.o
	cc -o thv2 thv2.o p1fxns.o

thv3: thv3.o p1fxns.o
	cc -o thv3 thv3.o p1fxns.o

thv4: thv4.o p1fxns.o
	cc -o thv4 thv4.o p1fxns.o

thv1.o: thv1.c p1fxns.c p1fxns.h
thv2.o: thv2.c p1fxns.c p1fxns.h
thv3.o: thv3.c p1fxns.c p1fxns.h
thv4.o: thv4.c p1fxns.c p1fxns.h

p1fxns.o: p1fxns.c p1fxns.h
	gcc -g -c -W -Wall p1fxns.c
