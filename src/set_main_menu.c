#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "set_main_menu.h"
#include "mystrerror.h"
#include "cameriere.h"
#include "lavoratore_cucina.h"
#include "manager.h"

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

	mysql_next_result(conn);

	menu_option* opts;

	if(val == 1) {
		OPTS_ALLOC(14);

		ASSIGN(0, "Assegna un tavolo a cliente", assegna_tavolo_a_cliente);
		ASSIGN(1, "Stampa scontrino", stampa_scontrino);
		ASSIGN(2, "Contrassegna scontrino come pagato", contrassegna_scontrino_pagato);
		ASSIGN(3, "Crea turno", crea_turno);
		ASSIGN(4, "Assegna turno", assegna_turno);
		ASSIGN(5, "Aggiungi al menu", aggiungi_al_menu);
		ASSIGN(6, "Aggiungi disponibilità ingrediente", aggiungi_disp_ingrediente);
		ASSIGN(7, "Aggiungi tavolo", aggiungi_tavolo);
		ASSIGN(8, "Aggiungi un altro manager", aggiungi_altro_manager);
		ASSIGN(9, "Aggiungi un cameriere", aggiungi_cameriere);
		ASSIGN(10, "Aggiungi un pizzaiolo", aggiungi_pizzaiolo);
		ASSIGN(11, "Aggiungi un barman", aggiungi_barman);
		ASSIGN(12, "Visualizza entrate mensili", visualizza_entrate_mensili);
		ASSIGN(13, "Visualizza entrate giornaliere", visualizza_entrate_giornaliere);
	} else if(val == 2){
		OPTS_ALLOC(4);

		ASSIGN(0, "Prendi ordinazione da un tavolo", avvia_prenotazione);
		ASSIGN(1, "Visualizza stato tavoli assegnati", visualizza_stato_tavoli_assegnati);
		ASSIGN(2, "Visualizza ordini pronti da consegnare", visualizza_ordini_da_consegnare);
		ASSIGN(3, "Contrassegna un ordine come consegnato", contrassegna_ordine_come_consegnato);
	} else if(val == 3) {
		OPTS_ALLOC(3);

		ASSIGN(0, "Visualizza stato ordini", visualizza_stato_ordini);
		ASSIGN(1, "Prendi in carico un ordine in particolare", prendi_in_carico_ordine);
		ASSIGN(2, "Contrassegna l'ordine come espletato", espleta_ordine);
	}
	else {
		myerror("unknown value for GetMyUserRole()!");
	}

	return opts;
}

