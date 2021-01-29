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

#define INIT_MYSQL_TIME_ONLYDATE(name, d, m, y) \
	MYSQL_TIME name; \
	memset(&name, 0, sizeof(name)); \
	name.day = d; \
	name.month = m; \
	name.year = y

#define INIT_MYSQL_TIME_DATETIME(name, d, m, y, h, min, s) \
	INIT_MYSQL_TIME_ONLYDATE(name, d, m, y); \
	name.hour = h; \
	name.minute = min; \
	name.second = s

#define __basic_set_inout_param_type(par, idx, ptr, buflen, buftype) \
	par[idx].buffer = ptr;                                             \
	par[idx].buffer_type = buftype;                                    \
	par[idx].buffer_length = buflen

#define __basic_set_inout_param_int_or_date(idx, buf, subtype, params) \
	__basic_set_inout_param_type(params, idx, buf, sizeof(*buf), subtype)

#define __basic_mysql_stmt_bind(fun, stmt, params) \
	if (fun(stmt, params)) { \
		MYSQL_STMT_BASIC_PRINTERROR_EXIT(#fun, stmt); \
	}

MYSQL_STMT* init_and_prepare_stmt(const char* query);
mybool execute_stmt(MYSQL_STMT* stmt);
void close_everything_stmt(MYSQL_STMT* stmt);

/* inout params */

#define set_in_param_string(idx, buf, params) \
	__basic_set_inout_param_type(params, idx, buf, strlen(buf), MYSQL_TYPE_VAR_STRING)

#define set_out_param_string(idx, buf, params) \
  	__basic_set_inout_param_type(params, idx, buf, sizeof(buf), MYSQL_TYPE_VAR_STRING)

#define set_inout_param_int(idx, buf, params) \
	__basic_set_inout_param_int_or_date(idx, buf, MYSQL_TYPE_LONG, params)

#define set_inout_param_tinyint(idx, buf, params) \
	__basic_set_inout_param_int_or_date(idx, buf, MYSQL_TYPE_TINY, params)

#define set_inout_param_smallint(idx, buf, params) \
	__basic_set_inout_param_int_or_date(idx, buf, MYSQL_TYPE_SHORT, params)

#define set_inout_param_double(idx, buf, params) \
	__basic_set_inout_param_type(params, idx, buf, 0, MYSQL_TYPE_DOUBLE)

#define set_inout_param_mybool(idx, buf, params) \
	set_inout_param_tinyint(idx, buf, params)

#define set_in_param_null(idx, params) \
	params[idx].is_null_value = TRUE

#define set_inout_param_date(idx, buf, params) \
	__basic_set_inout_param_int_or_date(idx, buf, MYSQL_TYPE_DATE, params)

#define set_inout_param_datetime(idx, buf, params) \
	__basic_set_inout_param_int_or_date(idx, buf, MYSQL_TYPE_TIMESTAMP, params)

/* bind */

#define bind_param_stmt(stmt, params) \
	__basic_mysql_stmt_bind(mysql_stmt_bind_param, stmt, params)

#define bind_result_stmt(stmt, params) \
	__basic_mysql_stmt_bind(mysql_stmt_bind_result, stmt, params)

/* close */

#define close_only_stmt(stmt) \
	if (mysql_stmt_close(stmt)) { \
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_stmt_close"); \
	}

/* fetch */

#define begin_fetch_stmt(stmt) \
	{ \
		int stmt_fetch_error; \
		for(int i = 0; !(stmt_fetch_error = mysql_stmt_fetch(stmt)); ++i) {

#define end_fetch_stmt() \
  		} \
  		if (stmt_fetch_error != MYSQL_NO_DATA) { \
		  	printf("%d\n", stmt_fetch_error == MYSQL_DATA_TRUNCATED); \
    		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_fetch", stmt); \
        } \
	}

#endif