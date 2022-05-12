
# author        Oliver Blaser
# date          12.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser


CC = g++
LINK = g++

CFLAGS = -c -I../../../include --std=c++17 -O3 -Wall -pedantic
LFLAGS = -O3 -Wall -pedantic

OBJS = main.o gpio.o uart.o
EXE = rpihal-system-test-uart-hdsp

BUILDDATE = $(shell date +"%Y-%m-%d-%H-%M")




$(EXE): $(OBJS)
	$(LINK) $(LFLAGS) -o $(EXE) $(OBJS)

main.o: main.cpp ../../../include/rpihal/gpio.h  ../../../include/rpihal/uart.h
	$(CC) $(CFLAGS) main.cpp

gpio.o: ../../../src/gpio.c ../../../include/rpihal/gpio.h
	$(CC) $(CFLAGS) ../../../src/gpio.c

uart.o: ../../../src/uart.c ../../../include/rpihal/uart.h
	$(CC) $(CFLAGS) ../../../src/uart.c

all: $(EXE)
	

run: $(EXE)
	@echo ""
	@echo "\033[38;5;27m--================# run #================--\033[39m"
	./$(EXE)

clean:
	rm $(OBJS)
	rm $(EXE)
