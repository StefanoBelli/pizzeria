#ifndef SET_MAIN_MENU_H
#define SET_MAIN_MENU_H

#include <mysql.h>
#include "interactive.h"

#define FREE_MENU_OPTION(ptr, len) { \
	for(int i = 0; i < len; ++i) { \
		free(ptr[i].option); \
		ptr[i].option = NULL; \
	} \
	free(ptr); \
	ptr = NULL; \
}

menu_option* set_main_menu_by_user_role(int* opts_len, MYSQL* conn);

#endif
