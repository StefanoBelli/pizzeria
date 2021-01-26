#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <mysql.h>

typedef struct {
	char* username;
	MYSQL* db_conn;
} config;

config cfg;
void exit_and_close(int code);

#endif