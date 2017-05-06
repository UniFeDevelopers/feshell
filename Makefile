all: feshell

ls_lib.o: ./src/ls/ls_lib.c
	gcc -Wall -c ./src/ls/ls_lib.c
	mv ls_lib.o src/ls/

feshell_lib.o: ./src/feshell_lib.c
	gcc -Wall -c ./src/feshell_lib.c
	mv feshell_lib.o src/

feshell.o: ./src/feshell.c
	gcc -Wall -c ./src/feshell.c
	mv feshell.o src/

feshell: ls_lib.o feshell_lib.o feshell.o
	gcc -Wall -o feshell ./src/ls/ls_lib.o ./src/feshell_lib.o ./src/feshell.o
	mv feshell bin/

clean:
	rm src/*.o
	rm src/ls/*.o
