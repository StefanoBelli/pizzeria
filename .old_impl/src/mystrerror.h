#ifndef MYSTRERROR_H
#define MYSTRERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <mysql.h>

#define myerror(msg) { \
	fprintf(stderr, "logic error: %s\n", msg); \
	exit(EXIT_FAILURE); \
}

#define eprint_exit(msg) { \
	perror(msg); \
	exit(EXIT_FAILURE); \
}

#define __mysql_strerror(msg, conn) { \
	fprintf (stderr, "(%s) mysql_error for %u (%s): %s\n", \
			msg, \
			mysql_errno (conn), \
			mysql_sqlstate(conn), \
			mysql_error (conn)); \
}

#define mysql_strerror_exit(msg, conn) { \
	__mysql_strerror(msg, conn); \
	mysql_close(conn); \
	exit(EXIT_FAILURE); \
}

#define __mysql_stmt_strerror(msg, stmt) { \
	fprintf (stderr, "(%s) mysql_stmt_error for %u (%s): %s\n", \
			msg, \
			mysql_stmt_errno (stmt), \
			mysql_stmt_sqlstate(stmt), \
			mysql_stmt_error (stmt)); \
}

#define mysql_stmt_strerror_exit(msg, stmt, conn) { \
	__mysql_stmt_strerror(msg, stmt); \
	mysql_close(conn); \
	exit(EXIT_FAILURE); \
}

#endif

