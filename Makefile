all: feshell

ls_lib.o: ./src/ls/ls_lib.c
	gcc -Wall -c ./src/ls/ls_lib.c 
	mv ls_lib.o ./src/ls/

parse_lib.o: ./src/parse_lib/parse_lib.c
	gcc -Wall -c ./src/parse_lib/parse_lib.c
	mv parse_lib.o ./src/parse_lib

feshell_lib.o: ./src/feshell_lib.c
	gcc -Wall -c ./src/feshell_lib.c
	mv feshell_lib.o src/

feshell.o: ./src/feshell.c
	gcc -Wall -c ./src/feshell.c
	mv feshell.o src/

feshell.test.o: ./src/feshell.test.c
	gcc -Wall -c ./src/feshell.test.c
	mv feshell.test.o src/

feshell: ls_lib.o parse_lib.o feshell_lib.o feshell.o
	gcc -Wall -o feshell ./src/ls/ls_lib.o ./src/parse_lib/parse_lib.o ./src/feshell_lib.o ./src/feshell.o -lm
	mv feshell bin/

test: ls_lib.o parse_lib.o feshell_lib.o feshell.test.o
	gcc -Wall -o test ./src/ls/ls_lib.o ./src/parse_lib/parse_lib.o ./src/feshell_lib.o ./src/feshell.test.o -lm
	mv test bin/

clean:
	rm src/*.o
	rm src/ls/*.o
	rm src/parse_lib/*.o
