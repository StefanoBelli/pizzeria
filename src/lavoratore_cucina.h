#ifndef LAVORATORE_CUCINA_H
#define LAVORATORE_CUCINA_H

#include <mysql.h>

void visualizza_stato_ordini(MYSQL*);
void prendi_in_carico_ordine(MYSQL*);
void espleta_ordine(MYSQL*);


#endif
