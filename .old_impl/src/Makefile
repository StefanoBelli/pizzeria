CC = gcc
CFLAGS = $(shell mysql_config --include) -W -Wall -Wextra
LIBS = $(shell mysql_config --libs)

SRC = main.c \
	  interactive.c \
	  set_main_menu.c \
	  lavoratore_cucina.c \
	  manager.c \
	  cameriere.c

OUTPUT = pizzeria

all:
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(OUTPUT)

debug:
	$(CC) $(CFLAGS) $(LIBS) $(SRC) -o $(OUTPUT) -g

clean:
	rm -v $(OUTPUT)
