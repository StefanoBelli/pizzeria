#ifndef MYSQL_UTILS_H
#define MYSQL_UTILS_H

#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global_config.h"
#include "mybool.h"

#define MYSQL_BASIC_PRINTERROR(msg) \
	(fprintf(stderr, "(%s) Error %u: %s\n", \
	msg, \
	mysql_errno(cfg.db_conn), \
	mysql_error(cfg.db_conn)))

//always failure
#define MYSQL_BASIC_PRINTERROR_EXIT(msg) \
	{ \
		MYSQL_BASIC_PRINTERROR(msg); \
		close_and_exit(EXIT_FAILURE); \
	}

#define MYSQL_STMT_BASIC_PRINTERROR(msg, stmt) \
	(fprintf(stderr, "(%s) Error %u (%s): %s\n", \
			msg, \
			mysql_stmt_errno (stmt), \
			mysql_stmt_sqlstate(stmt), \
			mysql_stmt_error (stmt)))

// always failure
#define MYSQL_STMT_BASIC_PRINTERROR_EXIT(msg, stmt) \
  {                                      \
    MYSQL_STMT_BASIC_PRINTERROR(msg, stmt);         \
    close_and_exit(EXIT_FAILURE);        \
  }

#define RESET_MYSQL_BIND(name) \
	memset(name, 0, sizeof(name))

#define INIT_MYSQL_BIND(name, n) \
	MYSQL_BIND name[n]; \
	RESET_MYSQL_BIND(name)

MYSQL_STMT* init_and_prepare_stmt(const char* query);
void bind_param_stmt(MYSQL_STMT* stmt, MYSQL_BIND* params);
void bind_result_stmt(MYSQL_STMT* stmt, MYSQL_BIND* params);
mybool execute_stmt(MYSQL_STMT* stmt);
void fetch_stmt(MYSQL_STMT* stmt);
void close_everything_stmt(MYSQL_STMT* stmt);
void close_only_stmt(MYSQL_STMT* stmt);

void set_in_param_string(int i, const char* buf, MYSQL_BIND* params);
void set_in_param_onlydate(int i, int day, int month, int year,
                           MYSQL_BIND* params);
void set_in_param_int(int i, int* out, enum enum_field_types mysql_subtype,
                      MYSQL_BIND* params);

#endif