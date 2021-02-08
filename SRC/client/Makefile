CC = gcc
SRC = main.c myutils.c myezcvt.c parse_dbms_conn_config.c \
	login.c mysql_utils.c manager.c cameriere.c lavoratore_cucina.c
CFLAGS = -W -Wall -Wextra $(shell mysql_config --cflags)
CFLAGS_DEBUG = $(CFLAGS) -g -ggdb
CFLAGS_SAN = $(CFLAGS) -fsanitize=address -lasan
LIBS = $(shell mysql_config --libs)
OUT = pizzeria

all:
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(OUT) && echo ok || echo fail
	@echo "***"
	@echo per runnare: ./$(OUT) --username USERNAME --password PASSWORD
	@echo "***"

debug:
	$(CC) $(SRC) $(CFLAGS_DEBUG) $(LIBS) -o $(OUT) && echo ok || echo fail
	@echo "***"
	@echo per runnare: ./$(OUT) --username USERNAME --password PASSWORD
	@echo "***"

san:
	$(CC) $(SRC) $(CFLAGS_SAN) $(LIBS) -o $(OUT) && echo ok || echo fail
	@echo "***"
	@echo per runnare: ./$(OUT) --username USERNAME --password PASSWORD
	@echo "***"

clean:
	rm $(OUT)

.PHONY: all, debug, clean
