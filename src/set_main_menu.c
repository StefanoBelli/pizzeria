#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set_main_menu.h"
#include "mystrerror.h"

#define ASSIGN(idx, optstr, funptr) { \
	opts[idx].option = (char*) malloc(sizeof(char) * sizeof(optstr)); \
	if(opts[idx].option == NULL) { \
		eprint_exit("malloc"); \
	} \
	memcpy(opts[idx].option, optstr, sizeof(optstr)); \
	opts[idx].handler = funptr; \
}

#define OPTS_ALLOC(nopt) \
	*opts_len = nopt; \
	opts = (menu_option*) malloc(sizeof(menu_option) * nopt); \
	if(opts == NULL) { \
		eprint_exit("malloc"); \
	}

menu_option* set_main_menu_by_user_role(int *opts_len, MYSQL* conn) {
	if(mysql_query(conn, "call GetMyUserRole()") != 0) {
		mysql_strerror_exit("mysql_query", conn);
	}

	MYSQL_RES *result_set = mysql_store_result(conn);
	if(result_set == NULL) {
		mysql_strerror_exit("mysql_store_result", conn);
	}

	MYSQL_ROW row = mysql_fetch_row(result_set);
	char* endptr = NULL;
	int val = strtol(row[0], &endptr, 10);
	
	mysql_free_result(result_set);

	menu_option* opts;

	if(val == 1) {
		OPTS_ALLOC(14);

		ASSIGN(0, "Assegna un tavolo a cliente", NULL);
		ASSIGN(1, "Stampa scontrino", NULL);
		ASSIGN(2, "Contrassegna scontrino come pagato", NULL);
		ASSIGN(3, "Crea turno", NULL);
		ASSIGN(4, "Assegna turno", NULL);
		ASSIGN(5, "Aggiungi al menu", NULL);
		ASSIGN(6, "Aggiungi disponibilità ingrediente", NULL);
		ASSIGN(7, "Aggiungi tavolo", NULL);
		ASSIGN(8, "Aggiungi un altro manager", NULL);
		ASSIGN(9, "Aggiungi un cameriere", NULL);
		ASSIGN(10, "Aggiungi un pizzaiolo", NULL);
		ASSIGN(11, "Aggiungi un barman", NULL);
		ASSIGN(12, "Visualizza entrate mensili", NULL);
		ASSIGN(13, "Visualizza entrate giornaliere", NULL);
	} else if(val == 2){
		OPTS_ALLOC(4);

		ASSIGN(0, "Prendi ordinazione da un tavolo", NULL);
		ASSIGN(1, "Visualizza stato tavoli assegnati", NULL);
		ASSIGN(2, "Visualizza ordini pronti da consegnare", NULL);
		ASSIGN(3, "Contrassegna un ordine come consegnato", NULL);
	} else if(val == 3) {
		OPTS_ALLOC(3);

		ASSIGN(0, "Visualizza ordini da eseguire", NULL);
		ASSIGN(1, "Prendi in carico un ordine in particolare", NULL);
		ASSIGN(2, "Contrassegna l'ordine come espletato", NULL);
	}
	else {
		myerror("unknown value for GetMyUserRole()!");
	}

	return opts;
}

