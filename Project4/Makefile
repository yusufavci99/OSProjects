all: p1 p2 p3
p1: p1.c
	gcc -Wall -g -o p1 p1.c
p2: p2.c
	gcc -Wall -g -o p2 p2.c
p3: p3.c
	gcc -Wall -g -o p3 p3.c

clean: clean_p1 clean_p2 clean_p3

clean_p1:
	rm -fr p1 p1.o *-
clean_p2:
	rm -fr p2 p2.o *-
clean_p3:
	rm -fr p3 p3.o *-
