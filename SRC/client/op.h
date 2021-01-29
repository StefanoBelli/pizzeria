#ifndef OP_H
#define OP_H

#include "myutils.h"

#define checked_execute_stmt(stmt) \
	if (!execute_stmt(stmt)) { \
		puts("operazione annullata."); \
		close_only_stmt(stmt); \
		return FALSE; \
	}

#define checked_show_form(fields, fields_len)   \
	if (show_form(fields, fields_len) == FALSE) { \
		puts("operazione annullata.");              \
		return FALSE;                               \
	}

#define checked_date_check(day, month, year)   \
	if (date_check(day, month, year) == FALSE) { \
		puts("data inserita non valida!");         \
		puts("operazione annullata.");             \
		return FALSE;                              \
	}

#define check_affected_stmt_rows(status, stmt, msg, ...) \
	if(mysql_stmt_affected_rows(stmt) == 0) { \
		printf(msg, __VA_ARGS__); \
		status = FALSE; \
	}

#define __basic_form_field(fields, idx, field_nm, expected_min, expected_max, \
                           output_sz, output_buf, output_ty)                  \
  	fields[idx].field_name = field_nm;                                          \
  	fields[idx].expected_min_len = expected_min;                                \
  	fields[idx].expected_max_len = expected_max;                                \
  	fields[idx].output = output_buf;                                            \
  	fields[idx].output_type = output_ty;                                        \
  	fields[idx].output_size = output_sz;

#define string_form_field(f, i, n, mi, ma, os, ob) \
  	__basic_form_field(f, i, n, mi, ma, os, ob, STRING)

#define int_form_field(f, i, n, mi, ma, ob) \
  	__basic_form_field(f, i, n, mi, ma, 0, ob, INTEGER)

#define double_form_field(f, i, n, mi, ma, ob) \
	__basic_form_field(f, i, n, mi, ma, 0, ob, DOUBLE)

#define mybool_form_field(f, i, n, ob) \
	__basic_form_field(f, i, n, 1, 3, 0, ob, MYBOOL)

mybool manager_crea_nuovo_utente();
mybool manager_ripristina_password_utente_esistente();
mybool manager_aggiungi_nuovo_tavolo();
mybool manager_aggiungi_nuovo_ingrediente();
mybool manager_aggiungi_prodotto_del_menu();
mybool manager_associa_prodotto_e_ingrediente();
mybool manager_crea_turno();
mybool manager_rimuovi_prodotto_del_menu();
mybool manager_rimuovi_ingrediente();
mybool manager_rimuovi_prodotto_e_ingrediente();
mybool manager_assegna_turno(); //TODO -- FARE
mybool manager_visualizza_turni();
mybool manager_visualizza_utenti();
mybool manager_visualizza_tavoli();

//Visualizza menu
//Visualizza situazione ingredienti
//Visualizza prodotti e ingredienti

#endif