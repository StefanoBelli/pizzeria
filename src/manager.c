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
	param_bind[3].length = 0;
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

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_al_menu_bar( MYSQL *conn)
{
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

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void crea_composizione( MYSQL *conn)
{
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

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_disp_ingrediente( MYSQL *conn)
{
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

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void aggiungi_ingrediente( MYSQL *conn)
{
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

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void togli_dal_menu( MYSQL *conn)
{
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
	MYSQL_STMT *stmt = mysql_stmt_init(conn);
	if (stmt == NULL)
	{
		mysql_strerror_exit("mysql_stmt_init", conn);
	}

	if (mysql_stmt_prepare(stmt,
						   "call OttieniTavoloDisponibile(?,?,?,?)",
						   sizeof("call OttieniTavoloDisponibile(?,?,?,?)")))
	{
		mysql_stmt_strerror_exit("mysql_stmt_prepare", stmt, conn);
	}

	if (mysql_stmt_close(stmt))
	{
		mysql_strerror_exit("mysql_stmt_close", conn);
	}
}

void crea_turno( MYSQL *conn)
{
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
