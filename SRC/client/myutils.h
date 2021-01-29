#ifndef MYUTILS_H
#define MYUTILS_H

#include "mybool.h"
#include "role.h"

typedef mybool (*entry_handler_fpt)();

typedef struct {
	char* entry;
	entry_handler_fpt handler;
} menu_entry;

typedef enum {
	STRING,
	MYBOOL,
	INTEGER,
	DOUBLE
} form_output_cvt_type;

typedef struct {
	char* field_name;
	int expected_min_len;
	int expected_max_len;
	void* output;
	form_output_cvt_type output_type;
	int output_size;
} form_field;

#define clear() (printf("\e[1;1H\e[2J"))

mybool show_menu();
mybool show_form(const form_field* fields, int fields_len);
mybool date_check(int day, int month, int year);
role str_to_role(char* role);
const char* role_to_str(role r);

#endif