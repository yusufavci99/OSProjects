all: mv mvp mvt
mv: mv.c
	gcc -Wall -g -o mv mv.c
mvp: mvp.c
	gcc -Wall -g -o mvp mvp.c
mvt: mvt.c
	gcc -Wall -g -o mvt mvt.c -lpthread

clean: cleanmv cleanmvp cleanmvt

cleanmv:
	rm -fr mv mv.o *-
cleanmvp:
	rm -fr mvp mvp.o *-
cleanmvt:
	rm -fr mvt mvt.o *-
