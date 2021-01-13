#include <string.h>

#include "manager.h"
#include "interactive.h"
#include "mystrerror.h"

static void __generic_aggiungi_lavoratore(char *query, size_t query_len,  MYSQL *conn)
{
	char nome[22] = {0};
	char cognome[22] = {0};
	char comuneResidenza[36] = {0};
	char giornoDataNascita[4] = {0};
	char meseDataNascita[4] = {0};
	char annoDataNascita[6] = {0};
	char comuneNascita[36] = {0};
	char cf[18] = {0};
	char chosenUsername[12] = {0};
	char password[32] = {0};

	form_entry entries[10] = {
		{"Nome", nome, 22},
		{"Cognome", cognome, 22},
		{"Comune residenza", comuneResidenza, 36},
		{"Giorno data di nascita", giornoDataNascita, 4},
		{"Mese data di nascita", meseDataNascita, 4},
		{"Anno data di nascita", annoDataNascita, 6},
		{"Comune nascita", comuneNascita, 36},
		{"Codice fiscale", cf, 18},
		{"Scegli username (max 10 caratteri)", chosenUsername, 12},
		{"Scegli password (max 30 caratteri)", password, 32}};

	compile_form(entries, 10);
	if (display_form(entries, 10) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	size_t nome_len = strlen(nome);
	size_t cognome_len = strlen(cognome);
	size_t comuneResidenza_len = strlen(comuneResidenza);
	size_t comuneNascita_len = strlen(comuneNascita);
	size_t cf_len = strlen(cf);
	size_t chosenUsername_len = strlen(chosenUsername);
	size_t password_len = strlen(password);

	// no error check
	MYSQL_TIME dataNascita;
	dataNascita.year = atoi(annoDataNascita);
	dataNascita.month = atoi(meseDataNascita);
	dataNascita.day = atoi(giornoDataNascita);

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt, query, query_len))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	MYSQL_BIND param_bind[8];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)cognome;
	param_bind[1].buffer_length = cognome_len;
	param_bind[1].buffer_type = MYSQL_TYPE_STRING;
	param_bind[1].length = &cognome_len;
	param_bind[1].is_null = NULL;

	param_bind[2].buffer = (char *)comuneResidenza;
	param_bind[2].buffer_length = comuneResidenza_len;
	param_bind[2].buffer_type = MYSQL_TYPE_STRING;
	param_bind[2].length = &comuneResidenza_len;
	param_bind[2].is_null = NULL;

	param_bind[3].buffer = (char *)&dataNascita;
	param_bind[3].buffer_length = sizeof(dataNascita);
	param_bind[3].buffer_type = MYSQL_TYPE_DATE;
	param_bind[3].length = NULL;
	param_bind[3].is_null = NULL;

	param_bind[4].buffer = (char *)comuneNascita;
	param_bind[4].buffer_length = comuneNascita_len;
	param_bind[4].buffer_type = MYSQL_TYPE_STRING;
	param_bind[4].length = &comuneNascita_len;
	param_bind[4].is_null = NULL;

	param_bind[5].buffer = (char *)cf;
	param_bind[5].buffer_length = cf_len;
	param_bind[5].buffer_type = MYSQL_TYPE_STRING;
	param_bind[5].length = &cf_len;
	param_bind[5].is_null = NULL;

	param_bind[6].buffer = (char *)chosenUsername;
	param_bind[6].buffer_length = chosenUsername_len;
	param_bind[6].buffer_type = MYSQL_TYPE_STRING;
	param_bind[6].length = &chosenUsername_len;
	param_bind[6].is_null = NULL;

	param_bind[7].buffer = (char *)password;
	param_bind[7].buffer_length = password_len;
	param_bind[7].buffer_type = MYSQL_TYPE_STRING;
	param_bind[7].length = &password_len;
	param_bind[7].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind))
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt))
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	}
	else
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void stampa_scontrino( MYSQL *conn)
{
}

void contrassegna_scontrino_pagato( MYSQL *conn)
{
}

void aggiungi_al_menu_pizzeria( MYSQL *conn)
{
	char nome[32] = { 0 };
	char costoUnitario[21] = { 0 };

	form_entry entries[2] = {
		{"Nome prodotto menu", nome, 32},
		{"Costo unitario", costoUnitario, 21}
	};

	compile_form(entries, 2);
	if (display_form(entries, 2) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call AggiungiProdottoPizzeria(?,?)",
						   sizeof("call AggiungiProdottoPizzeria(?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	float costoUnitarioFloat = atof(costoUnitario);
	size_t nome_len = strlen(nome);

	MYSQL_BIND param_bind[2];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&costoUnitarioFloat;
	param_bind[1].buffer_type = MYSQL_TYPE_FLOAT;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind))
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt))
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	}
	else
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_al_menu_bar( MYSQL *conn)
{
	char nome[32] = { 0 };
	char costoUnitario[21] = { 0 };
	char isAlcolico[10] = { 0 };

	form_entry entries[3] = {
		{"Nome prodotto menu", nome, 32},
		{"Costo unitario", costoUnitario, 21},
		{"Alcolico? (si/no)", isAlcolico, 10}
	};

	compile_form(entries, 3);
	if (display_form(entries, 3) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call AggiungiProdottoBar(?,?,?)",
						   sizeof("call AggiungiProdottoBar(?,?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	isAlcolico[0] = tolower(isAlcolico[0]);
	isAlcolico[1] = tolower(isAlcolico[1]);

	int isAlcolicoBoolean = !strcmp(isAlcolico, "si");
	float costoUnitarioFloat = atof(costoUnitario);
	size_t nome_len = strlen(nome);

	MYSQL_BIND param_bind[3];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&costoUnitarioFloat;
	param_bind[1].buffer_type = MYSQL_TYPE_FLOAT;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	param_bind[2].buffer = (char *)&isAlcolicoBoolean;
	param_bind[2].buffer_type = MYSQL_TYPE_TINY;
	param_bind[2].length = NULL;
	param_bind[2].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind))
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt))
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	}
	else
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void crea_composizione( MYSQL *conn)
{
	char nome[32] = { 0 };
	char nomeIng[32] = { 0 };

	form_entry entries[2] = {
		{"Nome prodotto menu", nome, 32},
		{"Ingrediente di base", nomeIng, 32}
	};

	compile_form(entries, 2);
	if (display_form(entries, 2) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call CreaComposizione(?,?)",
						   sizeof("call CreaComposizione(?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	size_t nome_len = strlen(nome);
	size_t nomeIng_len = strlen(nomeIng);

	MYSQL_BIND param_bind[2];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)nomeIng;
	param_bind[1].buffer_length = nomeIng_len;
	param_bind[1].buffer_type = MYSQL_TYPE_STRING;
	param_bind[1].length = &nomeIng_len;
	param_bind[1].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind))
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt))
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	}
	else
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_disp_ingrediente( MYSQL *conn)
{
	char nome[22] = { 0 };
	char disponibilita[21] = { 0 };

	form_entry entries[2] = {
		{"Nome", nome, 22},
		{"Incremento di", disponibilita, 21}
	};

	compile_form(entries, 2);
	if (display_form(entries, 2) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}
	
	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call AumentaScorteIngrediente(?,?)",
						   sizeof("call AumentaScorteIngrediente(?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	// no error checking
	size_t nome_len = strlen(nome);
	int disponibilitaInt = atoi(disponibilita);

	MYSQL_BIND param_bind[2];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&disponibilitaInt;
	param_bind[1].buffer_type = MYSQL_TYPE_LONG;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt)) 
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_ingrediente( MYSQL *conn)
{
	char nome[22] = { 0 };
	char disponibilita[21] = { 0 };
	char costoAlKg[21] = { 0 };

	form_entry entries[3] = {
		{"Nome", nome, 22},
		{"Disponibilità iniziale", disponibilita, 21},
		{"Costo al kg", costoAlKg, 21}
	};

	compile_form(entries, 3);
	if (display_form(entries, 3) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call CreaIngrediente(?,?,?)",
						   sizeof("call CreaIngrediente(?,?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	// no error checking
	size_t nome_len = strlen(nome);
	int disponibilitaInt = atoi(disponibilita);
	float costoAlKgFloat = atof(costoAlKg);

	MYSQL_BIND param_bind[3];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&disponibilitaInt;
	param_bind[1].buffer_type = MYSQL_TYPE_LONG;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	param_bind[2].buffer = (char *)&costoAlKgFloat;
	param_bind[2].buffer_type = MYSQL_TYPE_FLOAT;
	param_bind[2].length = NULL;
	param_bind[2].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt)) 
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void togli_dal_menu( MYSQL *conn)
{
	char nome[22] = { 0 };

	form_entry entries[1] = {
		{"Nome", nome, 22}
	};

	compile_form(entries, 1);
	if (display_form(entries, 1) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call TogliDalMenu(?)",
						   sizeof("call TogliDalMenu(?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	size_t nome_len = strlen(nome);

	MYSQL_BIND param_bind[1];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt))
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void visualizza_entrate_mensili( MYSQL *conn)
{
	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call VisualizzaEntrateMensili()",
						   sizeof("call VisualizzaEntrateMensili()")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void visualizza_entrate_giornaliere( MYSQL *conn)
{
	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call VisualizzaEntrateGiornaliere()",
						   sizeof("call VisualizzaEntrateGiornaliere()")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void assegna_tavolo_a_cliente( MYSQL *conn)
{
	char nome[22] = { 0 };
	char cognome[21] = { 0 };
	char numCommensali[21] = { 0 };

	form_entry entries[3] = {
		{"Nome", nome, 22},
		{"Cognome", cognome, 21},
		{"Numero commensali", numCommensali, 21}
	};

	compile_form(entries, 3);
	if (display_form(entries, 3) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call OttieniTavoloDisponibile(?,?,?)",
						   sizeof("call OttieniTavoloDisponibile(?,?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	//no error checking
	int numCommensaliInt = atoi(numCommensali);

	size_t nome_len = strlen(nome);
	size_t cognome_len = strlen(cognome);

	MYSQL_BIND param_bind[3];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)nome;
	param_bind[0].buffer_length = nome_len;
	param_bind[0].buffer_type = MYSQL_TYPE_STRING;
	param_bind[0].length = &nome_len;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)cognome;
	param_bind[1].buffer_length = cognome_len;
	param_bind[1].buffer_type = MYSQL_TYPE_STRING;
	param_bind[1].length = &cognome_len;
	param_bind[1].is_null = NULL;

	param_bind[2].buffer = (char *)&numCommensaliInt;
	param_bind[2].buffer_type = MYSQL_TYPE_SHORT;
	param_bind[2].length = NULL;
	param_bind[2].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt)) 
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);

		if (mysql_stmt_close(stmt))
		{
			mysql_strerror_exit("mysql_stmt_close", conn);
		}

		return;
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	int result;

	MYSQL_BIND res_bind[1];
	memset(res_bind, 0, sizeof(res_bind));

	res_bind[0].buffer = (char *)&result;
	res_bind[0].buffer_type = MYSQL_TYPE_LONG;
	res_bind[0].length = NULL;
	res_bind[0].is_null = NULL;

	if (mysql_stmt_bind_result(stmt, res_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_result", stmt, conn);
	}

	if (mysql_stmt_store_result(stmt))
	{
		mysql_stmt_strerror_exit("mysql_stmt_store_result", stmt, conn);
	}

	mysql_stmt_fetch(stmt);

	printf("Tavolo correttamente assegnato: %d\n", result);

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void crea_turno( MYSQL *conn)
{
	char giornoInizio[4] = { 0 };
	char meseInizio[4] = { 0 };
	char annoInizio[6] = { 0 };
	char oraInizio[4] = { 0 };
	char minInizio[4] = { 0 };
	char giornoFine[4] = { 0 };
	char meseFine[4] = { 0 };
	char annoFine[6] = { 0 };
	char oraFine[4] = { 0 };
	char minFine[4] = { 0 };

	form_entry entries[10] = {
		{"Giorno inizio", giornoInizio, 4},
		{"Mese inizio", meseInizio, 4},
		{"Anno inizio", annoInizio, 6},
		{"Ora inizio", oraInizio, 4},
		{"Minuti inizio", minInizio, 4},
		{"Giorno fine", giornoFine, 4},
		{"Mese fine", meseFine, 4},
		{"Anno fine", annoFine, 6},
		{"Ora fine", oraFine, 4},
		{"Minuti fine", minFine, 4}
	};

	compile_form(entries, 10);
	if (display_form(entries, 10) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call CreaTurno(?,?)",
						   sizeof("call CreaTurno(?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	MYSQL_TIME dataOraInizio;
	dataOraInizio.year = atoi(giornoInizio);
	dataOraInizio.month = atoi(meseInizio);
	dataOraInizio.day = atoi(annoInizio);
	dataOraInizio.hour = atoi(oraInizio);
	dataOraInizio.minute = atoi(minInizio);

	MYSQL_TIME dataOraFine;
	dataOraFine.year = atoi(giornoFine);
	dataOraFine.month = atoi(meseFine);
	dataOraFine.day = atoi(annoFine);
	dataOraFine.hour = atoi(oraFine);
	dataOraFine.minute = atoi(minFine);
	
	MYSQL_BIND param_bind[2];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)&dataOraInizio;
	param_bind[0].buffer_length = sizeof(dataOraInizio);
	param_bind[0].buffer_type = MYSQL_TYPE_TIMESTAMP;
	param_bind[0].length = NULL;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&dataOraFine;
	param_bind[1].buffer_length = sizeof(dataOraFine);
	param_bind[1].buffer_type = MYSQL_TYPE_TIMESTAMP;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt)) 
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void assegna_turno( MYSQL *conn)
{
	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call AssegnaTurno(?,?,?)",
						   sizeof("call AssegnaTurno(?,?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_tavolo( MYSQL *conn)
{
	char numTavolo[21] = { 0 };
	char numCommensali[21] = { 0 };

	form_entry entries[2] = {
		{"Numero del nuovo tavolo", numTavolo, 21},
		{"Numero massimo di commensali", numCommensali, 21}
	};

	compile_form(entries, 2);
	if (display_form(entries, 2) == FALSE)
	{
		puts("Operazione annullata.");
		return;
	}

	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call CreaTavolo(?,?)",
						   sizeof("call CreaTavolo(?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	int numTavoloInt = atoi(numTavolo);
	int numCommensaliInt = atoi(numCommensali);

	MYSQL_BIND param_bind[2];
	memset(param_bind, 0, sizeof(param_bind));

	param_bind[0].buffer = (char *)&numTavoloInt;
	param_bind[0].buffer_type = MYSQL_TYPE_LONG;
	param_bind[0].length = NULL;
	param_bind[0].is_null = NULL;

	param_bind[1].buffer = (char *)&numCommensaliInt;
	param_bind[1].buffer_type = MYSQL_TYPE_LONG;
	param_bind[1].length = NULL;
	param_bind[1].is_null = NULL;

	if (mysql_stmt_bind_param(stmt, param_bind)) 
	{
		mysql_stmt_strerror_exit("mysql_stmt_bind_param", stmt, conn);
	}

	if (mysql_stmt_execute(stmt)) 
	{
		__mysql_stmt_strerror("mysql_stmt_execute", stmt);
	} 
	else 
	{
		puts("Ok, fatto!");
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_altro_manager( MYSQL *conn)
{
	__generic_aggiungi_lavoratore(
		"call CreaManager(?,?,?,?,?,?,?,?)",
		sizeof("call CreaManager(?,?,?,?,?,?,?,?)"),
		conn);
}

void aggiungi_cameriere( MYSQL *conn)
{
	__generic_aggiungi_lavoratore(
		"call CreaCameriere(?,?,?,?,?,?,?,?)",
		sizeof("call CreaCameriere(?,?,?,?,?,?,?,?)"),
		conn);
}

void aggiungi_pizzaiolo( MYSQL *conn)
{
	__generic_aggiungi_lavoratore(
		"call CreaPizzaiolo(?,?,?,?,?,?,?,?)",
		sizeof("call CreaPizzaiolo(?,?,?,?,?,?,?,?)"),
		conn);
}

void aggiungi_barman( MYSQL *conn)
{
	__generic_aggiungi_lavoratore(
		"call CreaBarman(?,?,?,?,?,?,?,?)",
		sizeof("call CreaBarman(?,?,?,?,?,?,?,?)"),
		conn);
}
