# Simple makefile that will compile all the .c files in the current directory
# and make an executable called "main.exe" in the same directory
# to build the project, type "make" in the terminal
# to remove all the non-source files, type "make clean" in the terminal
CC = gcc

SOURCES = $(shell echo *.c)
HEADERS = $(shell echo *.h)

OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

main: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

scanner.o: token.h debug.h
token.o: token.h debug.h
lexer.o: token.h scanner.h global_defs.h debug.h


.PHONY: clean

clean:
	rm *.o *.exe
