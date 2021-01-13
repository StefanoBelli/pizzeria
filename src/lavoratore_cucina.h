#ifndef LAVORATORE_CUCINA_H
#define LAVORATORE_CUCINA_H

#include <mysql.h>

void visualizza_stato_ordini(char*, MYSQL*);
void prendi_in_carico_ordine(char*, MYSQL*);
void espleta_ordine(char*, MYSQL*);


#endif