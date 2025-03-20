CC = gcc
ARGS = -Wall -Wextra -Werror
#ARGS = -Wall -Wextra

run: build
	./bin/program

build: clean
	mkdir bin/
	$(CC) $(ARGS) main.c src/* -Iinclude/ -o bin/program

clean:
	rm -rf bin/
