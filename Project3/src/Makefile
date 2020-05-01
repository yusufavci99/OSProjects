all: app module1.o module2.o module3.o

app: module1.o module2.o module3.o
	gcc -g -Wall -no-pie -o app module1.o module2.o module3.o

module1.o: module1.c
	gcc -c -no-pie module1.c

module2.o: module2.c
	gcc -c -no-pie module2.c

module3.o: module3.c
	gcc -c -no-pie module3.c
