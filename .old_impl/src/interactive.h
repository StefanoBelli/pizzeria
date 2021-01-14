#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <mysql.h>
#include "boolean.h"

typedef void (*option_handler_fpt)(MYSQL*);

typedef struct {
	char* option;
	option_handler_fpt handler;
} menu_option;

typedef struct {
	const char* what;
	char* output;
	size_t output_len;
} form_entry;

void compile_form(form_entry*, int);
boolean display_form(form_entry*, int);
boolean show_menu(menu_option* opts, int opts_len, char* username, MYSQL* conn, char* menu_name);
	
#endif
