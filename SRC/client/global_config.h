#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <mysql.h>
#include "myutils.h"

typedef struct {
	char* username;
	MYSQL* db_conn;
	menu_entry* menu_entries;
	int menu_entries_len;
} config;

config cfg;
void exit_and_close(int code);

#endif