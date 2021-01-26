#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <mysql.h>

typedef struct {
	char* username;
	MYSQL* conn;
} config;

config cfg;

#endif