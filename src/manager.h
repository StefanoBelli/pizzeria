#ifndef MANAGER_H
#define MANAGER_H

#include <mysql.h>

void assegna_tavolo_a_cliente(char*, MYSQL*);
void stampa_scontrino(char*, MYSQL*);
void contrassegna_scontrino_pagato(char*, MYSQL*);
void crea_turno(char*, MYSQL*);
void assegna_turno(char*, MYSQL*);
void aggiungi_al_menu(char*, MYSQL*);
void aggiungi_disp_ingrediente(char*, MYSQL*);
void aggiungi_altro_manager(char*, MYSQL*);
void aggiungi_cameriere(char*, MYSQL*);
void aggiungi_pizzaiolo(char*, MYSQL*);
void aggiungi_barman(char*, MYSQL*);
void visualizza_entrate_mensili(char*, MYSQL*);
void visualizza_entrate_giornaliere(char*, MYSQL*);
void aggiungi_tavolo(char*, MYSQL*);

#endif