#include "mysql_utils.h"

#define INIT_MYSQL_TIME(name) \
  MYSQL_TIME name;            \
  memset(&name, 0, sizeof(name))

MYSQL_STMT* init_and_prepare_stmt(const char* query) {
	MYSQL_STMT* stmt = mysql_stmt_init(cfg.db_conn);
	if(stmt == NULL) {
		close_and_exit(EXIT_FAILURE);
	}

	if(mysql_stmt_prepare(stmt, query, strlen(query))) {
		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_prepare", stmt);
	}

	return stmt;
}

void bind_param_stmt(MYSQL_STMT* stmt, MYSQL_BIND* params) {
	if(mysql_stmt_bind_param(stmt, params)) {
		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_bind_param", stmt);
	}
}

void bind_result_stmt(MYSQL_STMT* stmt, MYSQL_BIND* params) {
	if(mysql_stmt_bind_result(stmt, params)) {
          MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_bind_result", stmt);
    }
}

mybool execute_stmt(MYSQL_STMT* stmt) {
  if (mysql_stmt_execute(stmt)) {
	  MYSQL_STMT_BASIC_PRINTERROR("mysql_stmt_execute", stmt);
	  return FALSE;
  }

  if (mysql_stmt_store_result(stmt)) {
	  MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_store_result", stmt);
  }

  return TRUE;
}

void fetch_stmt(MYSQL_STMT* stmt) {
	if(mysql_stmt_fetch(stmt)) {
		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_store_result", stmt);
    }
}

void close_everything_stmt(MYSQL_STMT* stmt) {
	//consuma tutti i result set non consumati
	int code;
	while(!(code=mysql_stmt_next_result(stmt))) {}

	if(code > 0) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_stmt_next_result");
	}
	//tutti i result set sono stati consumati

	if(mysql_stmt_close(stmt)) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_stmt_close");
	}
}

void close_only_stmt(MYSQL_STMT* stmt) {
	if (mysql_stmt_close(stmt)) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_stmt_close");
	}
}

void set_in_param_string(int i, const char* buf, MYSQL_BIND* params) {
	params[i].buffer = (void*) buf;
	params[i].buffer_type = MYSQL_TYPE_VAR_STRING;
	params[i].buffer_length = strlen(buf);
}

void set_in_param_int(int i, int* out, enum enum_field_types mysql_subtype, MYSQL_BIND* params) {
	params[i].buffer = (void*) out;
	params[i].buffer_type = mysql_subtype;
	params[i].buffer_length = sizeof(out);
}

void set_in_param_onlydate(int i, int day, int month, int year, MYSQL_BIND* params) {
	INIT_MYSQL_TIME(date);
	date.day = day;
	date.month = month;
	date.year = year;

	params[i].buffer = (void*) &date;
	params[i].buffer_type = MYSQL_TYPE_DATE;
	params[i].buffer_length = sizeof(MYSQL_TIME);
}