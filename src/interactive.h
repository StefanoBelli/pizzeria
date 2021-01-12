#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include "boolean.h"

typedef void (*option_handler_fpt)(char*, MYSQL*);

typedef struct {
	char* option;
	option_handler_fpt handler;
} menu_option;

boolean show_menu(const menu_option* opts, int opts_len, char* username, MYSQL* conn, char* menu_name);
	
#endif
