CC = gcc
SRC = main.c myutils.c myezcvt.c parse_dbms_conn_config.c login.c mysql_utils.c manager.c cameriere.c
CFLAGS = -W -Wall -Wextra $(shell mysql_config --cflags)
CFLAGS_DEBUG = $(CFLAGS) -g -ggdb
CFLAGS_SAN = $(CFLAGS) -fsanitize=address -lasan
LIBS = $(shell mysql_config --libs)
OUT = pizzeria

all:
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(OUT)

debug:
	$(CC) $(SRC) $(CFLAGS_DEBUG) $(LIBS) -o $(OUT)

san:
	$(CC) $(SRC) $(CFLAGS_SAN) $(LIBS) -o $(OUT)

clean:
	rm $(OUT)

.PHONY: all, debug, clean
