#ifndef MANAGER_H
#define MANAGER_H

#include <mysql.h>

void assegna_tavolo_a_cliente( MYSQL*);
void stampa_scontrino( MYSQL*);
void contrassegna_scontrino_pagato( MYSQL*);
void crea_turno( MYSQL*);
void assegna_turno( MYSQL*);
void aggiungi_al_menu_pizzeria( MYSQL*);
void aggiungi_al_menu_bar( MYSQL*);
void aggiungi_ingrediente( MYSQL*);
void aggiungi_disp_ingrediente( MYSQL*);
void aggiungi_altro_manager( MYSQL*);
void aggiungi_cameriere( MYSQL*);
void aggiungi_pizzaiolo( MYSQL*);
void aggiungi_barman( MYSQL*);
void visualizza_entrate_mensili( MYSQL*);
void visualizza_entrate_giornaliere( MYSQL*);
void aggiungi_tavolo( MYSQL*);
void togli_dal_menu( MYSQL*);
void crea_composizione( MYSQL*);

#endif
