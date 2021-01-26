#ifndef PARSE_DBMS_CONN_CONFIG_H
#define PARSE_DBMS_CONN_CONFIG_H

#include "mybool.h"

typedef struct {
	char* db_username;
	char* db_password;
	char* db_hostname;
	char* db_name;
	int db_port;
} dbms_conn_config;

mybool parse_dbms_conn_config(const char* path, dbms_conn_config* conf);

#endif