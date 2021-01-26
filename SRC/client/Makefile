CC = gcc
SRC = main.c myutils.c myezcvt.c parse_dbms_conn_config.c
CFLAGS = -W -Wall -Wextra $(shell mysql_config --cflags)
CFLAGS_DEBUG = $(CFLAGS) -g -ggdb
LIBS = $(shell mysql_config --libs)
OUT = pizzeria

all:
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(OUT)

debug:
	$(CC) $(SRC) $(CFLAGS_DEBUG) $(LIBS) -o $(OUT)

clean:
	rm $(OUT)

.PHONY: all, debug, clean
