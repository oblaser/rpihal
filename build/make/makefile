
# author        Oliver Blaser
# date          04.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser


CC = g++
LINK = g++

CFLAGS = -c -I../../include --std=c++17 -O3 --Wall -pedantic
LFLAGS = -O3 -Wall -pedantic

OBJS = main.o gpio.o
EXE = rpihal-system-test

BUILDDATE = $(shell date +"%Y-%m-%d-%H-%M")




$(EXE): $(OBJS)
	$(LINK) $(LFLAGS) -o $(EXE) $(OBJS)

main.o: ../../test/system/main.cpp ../../include/rpihal/gpio.h
	$(CC) $(CFLAGS) ../../test/system/main.cpp

gpio.o: ../../src/gpio.cpp ../../include/rpihal/gpio.h
	$(CC) $(CFLAGS) ../../src/gpio.cpp

all: $(EXE)
	

clean:
	rm $(OBJS)
	rm $(EXE)
