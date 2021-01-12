#ifndef MYSTRERROR_H
#define MYSTRERROR_H

#define eprint_exit(msg) { \
	perror(msg); \
	exit(EXIT_FAILURE); }

#define __mysql_strerror(msg, conn) { \
	fprintf(stderr, "mysql_error for %s: %s\n", msg, mysql_error(conn)); \
}

#define mysql_strerror_exit(msg, conn) { \
	__mysql_strerror(msg, conn); \
	mysql_close(conn); \
	exit(EXIT_FAILURE); \
}

#endif

