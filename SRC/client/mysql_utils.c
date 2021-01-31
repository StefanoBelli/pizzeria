#include "mysql_utils.h"

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

void close_everything_stmt(MYSQL_STMT* stmt) {
	//consuma tutti i result set non consumati
	int code;
	while(!(code=mysql_stmt_next_result(stmt))) {}

	if(code > 0) {
		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_next_result", stmt);
	}
	//tutti i result set sono stati consumati

	if(mysql_stmt_close(stmt)) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_stmt_close");
	}
}
