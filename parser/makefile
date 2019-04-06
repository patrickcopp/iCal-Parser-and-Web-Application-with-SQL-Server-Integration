#Patrick Copp
#1007602
CC = gcc
LIB = -Iinclude -Isrc -LCalendarParser.h -LLinkedListAPI.h -LmyFunctions.h
SRC = src/myFunctions.c src/LinkedListAPI.c src/CalendarParser.c
FLAG = -Wall -pedantic -g -std=c11

all: parser

parser: CalendarParser.o LinkedList.o myFunctions.o
	$(CC) -shared CalendarParser.o LinkedList.o myFunctions.o $(LIB) $(FLAG) -o libcal.so
	mv libcal.so ../
LinkedList.o: src/LinkedListAPI.c
	$(CC) src/LinkedListAPI.c -Iinclude $(LIB) $(FLAG) -c -fpic -o LinkedList.o

CalendarParser.o: $(SRC)
	$(CC) src/CalendarParser.c -Iinclude $(LIB) $(FLAG) -c -fpic -o CalendarParser.o

myFunctions.o: src/myFunctions.c
	$(CC) src/myFunctions.c -Iinclude $(LIB) $(FLAG) -c -fpic -o myFunctions.o

#test: src/test.c all
#	$(CC) $(SRC) src/test.c $(FLAG) $(LIB) -o test

clean:
	rm *.o *.so bin/*.o bin/*.so core
