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

#define checked_show_form(fields, fields_len) \
	if(show_form(fields, fields_len) == FALSE) { \
		puts("operazione annullata."); \
		return FALSE; \
	}

#define checked_date_check(day, month, year) \
	if(date_check(day, month, year) == FALSE) { \
		puts("data inserita non valida!"); \
		puts("operazione annullata."); \
		return FALSE; \
	}

#define __basic_form_field(fields, idx, field_nm, expected_min, expected_max, output_sz, output_buf, output_ty) \
	fields[idx].field_name = field_nm; \
	fields[idx].expected_min_len = expected_min; \
	fields[idx].expected_max_len = expected_max; \
	fields[idx].output = output_buf; \
	fields[idx].output_type = output_ty; \
	fields[idx].output_size = output_sz;

#define string_form_field(f, i, n, mi, ma, os, ob) \
	__basic_form_field(f, i, n, mi, ma, os, ob, STRING)

#define int_form_field(f, i, n, mi, ma, ob) \
  __basic_form_field(f, i, n, mi, ma, 0, ob, INTEGER)

mybool show_menu();
mybool show_form(const form_field* fields, int fields_len);
mybool date_check(int day, int month, int year);
role str_to_role(char* role);

#endif