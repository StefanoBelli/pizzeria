CC = gcc
CFLAGS = $(shell mysql_config --include) -W -Wall -Wextra -g
LIBS = $(shell mysql_config --libs)

SRC = main.c interactive.c set_main_menu.c

OUTPUT = pizzeria

all:
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(OUTPUT)

clean:
	rm -v $(OUTPUT)
