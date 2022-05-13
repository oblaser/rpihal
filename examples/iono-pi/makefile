
# author        Oliver Blaser
# date          13.05.2022
# copyright     MIT - Copyright (c) 2022 Oliver Blaser


CC = gcc
LINK = gcc

CFLAGS = -c -I../../include -O3 -Wall -pedantic
LFLAGS = -O3 -Wall -pedantic

OBJS = main.o gpio.o
EXE = rpihal-example-ionopi

BUILDDATE = $(shell date +"%Y-%m-%d-%H-%M")




$(EXE): $(OBJS)
	$(LINK) $(LFLAGS) -o $(EXE) $(OBJS)

main.o: main.c ../../include/rpihal/gpio.h
	$(CC) $(CFLAGS) main.c

gpio.o: ../../src/gpio.c ../../include/rpihal/gpio.h
	$(CC) $(CFLAGS) ../../src/gpio.c

all: $(EXE)
	

run: $(EXE)
	@echo ""
	@echo "\033[38;5;27m--================# run #================--\033[39m"
	./$(EXE)

clean:
	rm $(OBJS)
	rm $(EXE)
