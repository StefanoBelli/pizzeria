#ifndef OP_H
#define OP_H

#define checked_execute_stmt(stmt) \
	if (!execute_stmt(stmt)) { \
		puts("operazione annullata."); \
		close_only_stmt(stmt); \
		return FALSE; \
	}

#include "mybool.h"

mybool manager_crea_nuovo_utente();
mybool manager_ripristina_password_utente_esistente();

#endif