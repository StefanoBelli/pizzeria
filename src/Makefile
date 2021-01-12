CC = gcc
CFLAGS = $(shell mysql_config --include) -W -Wall -Wextra
LIBS = $(shell mysql_config --libs)

SRC = main.c interactive.c

OUTPUT = pizzeria

all:
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(OUTPUT)

clean:
	rm -v $(OUTPUT)
