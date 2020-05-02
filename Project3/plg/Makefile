all: comp lin

comp:
	gcc -c -no-pie test.c
lin: test.o
	gcc -Wall -no-pie -o test test.o

