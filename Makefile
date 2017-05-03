all: feshell

execLS.o: ./src/execLS.c
	gcc -Wall -c ./src/execLS.c
	mv execLS.o src/

feshell: execLS.o
	gcc -Wall -o feshell ./src/execLS.o
	mv feshell bin/

clean:
	rm src/*.o

