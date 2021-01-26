#ifndef MYSQL_UTILS_H
#define MYSQL_UTILS_H

#include <stdlib.h>
#include "global_config.h"

#define MYSQL_BASIC_PRINTERROR(msg) \
	(fprintf(stderr, "%s: %s\n",msg, mysql_error(cfg.db_conn)))

//always failure
#define MYSQL_BASIC_PRINTERROR_EXIT(msg) \
	{ \
		MYSQL_BASIC_PRINTERROR(msg); \
		exit_and_close(EXIT_FAILURE); \
	}

#endif