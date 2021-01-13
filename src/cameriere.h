#ifndef CAMERIERE_H
#define CAMERIERE_H

#include <mysql.h>

void avvia_prenotazione(char*, MYSQL*);
void visualizza_stato_tavoli_assegnati(char*, MYSQL*);
void visualizza_ordini_da_consegnare(char*, MYSQL*);
void contrassegna_ordine_come_consegnato(char*, MYSQL*);

#endif