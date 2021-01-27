#include "mysql_utils.h"
#include "op.h"

mybool manager_crea_nuovo_utente() {
	char nome[21] = { 0 };
    char cognome[21] = { 0 };
    char cf[17] = { 0 };
    char comuneResidenza[35] = { 0 };
    char comuneNascita[35] = { 0 };
    char username[11] = {0};
    char password[46] = {0};
    char ruolo[10] = {0};
    int giornoNascita;
    int meseNascita;
    int annoNascita;

	form_field fields[11];
	string_form_field(fields, 0, "Nome", 1, 20, 20, nome);
    string_form_field(fields, 1, "Cognome", 1, 20, 20, cognome);
    string_form_field(fields, 2, "Codice fiscale", 16, 16, 16, cf);
    string_form_field(fields, 3, "Comune di residenza", 1, 34, 34, comuneResidenza);
    string_form_field(fields, 4, "Comune di nascita", 1, 34, 34, comuneNascita);
	int_form_field(fields, 5, "Giorno di nascita", 1, 2, &giornoNascita);
	int_form_field(fields, 6, "Mese di nascita", 1, 2, &meseNascita);
	int_form_field(fields, 7, "Anno di nascita", 4, 4, &annoNascita);
	string_form_field(fields, 8, "Username", 1, 10, 10, username);
	string_form_field(fields, 9, "Password", 1, 45, 45, password);
	string_form_field(fields, 10, "Ruolo", 6, 9, 9, ruolo);

	checked_show_form(fields, 11);
	checked_date_check(giornoNascita, meseNascita, annoNascita);

	role r;
	if((r = str_to_role(ruolo)) == ROLE_UNKNOWN) {
		puts("ruolo sconosciuto. possibili valori:\n"
				" * manager\n"
				" * cameriere\n"
				" * pizzaiolo\n"
				" * barman");
		puts("operazione annullata.");
		return FALSE;
	}

	MYSQL_STMT* stmt = init_and_prepare_stmt("call RegistraUtente(?,?,?,?,?,?,?,?,?)");

	INIT_MYSQL_BIND(params, 9);
	set_in_param_string(0, username, params);
    set_in_param_string(1, nome, params);
    set_in_param_string(2, cognome, params);
    set_in_param_string(3, comuneResidenza, params);
	set_in_param_onlydate(4, giornoNascita, meseNascita, annoNascita, params);
	set_in_param_string(5, comuneNascita, params);
	set_in_param_string(6, cf, params);
	set_in_param_string(7, password, params);
	set_in_param_int(8, (int*)&r, MYSQL_TYPE_TINY, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
	return TRUE;
}

mybool manager_ripristina_password_utente_esistente() {
	char username[11] = { 0 };
	char newpasswd[46] = { 0 };

	form_field fields[2];
	string_form_field(fields, 0, "Username", 1, 10, 10, username);
	string_form_field(fields, 1, "Nuova password", 1, 45, 45, newpasswd);

	checked_show_form(fields, 2);

	MYSQL_STMT *stmt = init_and_prepare_stmt("call RipristinoPassword(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_in_param_string(0, username, params);
	set_in_param_string(1, newpasswd, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	mybool is_ok = TRUE;

	if(mysql_stmt_affected_rows(stmt) == 0) {
		printf("non esiste alcun utente che si chiami \"%s\"\n"
				"password non alterate.\n",username);
		is_ok = FALSE;
	}

	close_everything_stmt(stmt);
	return is_ok;
}