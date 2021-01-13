#ifndef CAMERIERE_H
#define CAMERIERE_H

#include <mysql.h>

void avvia_prenotazione(MYSQL*);
void visualizza_stato_tavoli_assegnati(MYSQL*);
void visualizza_ordini_da_consegnare(MYSQL*);
void contrassegna_ordine_come_consegnato(MYSQL*);

#endif
