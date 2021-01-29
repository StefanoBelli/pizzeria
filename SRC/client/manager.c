#include "mysql_utils.h"
#include "op.h"
#include "goodmalloc.h"

typedef struct {
	char comune_residenza[35];
	char comune_nascita[35];
	char nome[21];
	char cognome[21];
	char cf[17];
	char username[11];
	MYSQL_TIME data_nascita;
	role ruolo;
} utente;

typedef struct {
  	int giorno;
  	int mese;
  	int anno;
  	int ora;
  	int minuto;
} orario;

typedef struct {
  	orario inizio;
  	orario fine;
} turno;

typedef struct {
	int num_tavolo;
	int num_max_commensali;
	mybool is_occupato;
	mybool is_attivo;
} tavolo;

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

	INIT_MYSQL_TIME_ONLYDATE(nascita, giornoNascita, meseNascita, annoNascita);

	INIT_MYSQL_BIND(params, 9);
	set_in_param_string(0, username, params);
    set_in_param_string(1, nome, params);
    set_in_param_string(2, cognome, params);
    set_in_param_string(3, comuneResidenza, params);
	set_inout_param_date(4, &nascita, params);
	set_in_param_string(5, comuneNascita, params);
	set_in_param_string(6, cf, params);
	set_in_param_string(7, password, params);
	set_inout_param_tinyint(8, (int*)&r, params);
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
	check_affected_stmt_rows(is_ok, stmt, "non esiste alcun utente che si chiami \"%s\"\n"
											"password non alterate.\n", username);

	close_everything_stmt(stmt);
	return is_ok;
}

mybool manager_aggiungi_nuovo_tavolo() {
	int numero_tavolo;
	int max_commensali;
	
	form_field fields[2];
	int_form_field(fields, 0, "Numero del tavolo", 1, 5, &numero_tavolo);
    int_form_field(fields, 1, "Max commensali", 1, 3, &max_commensali);

	checked_show_form(fields, 2);

	MYSQL_STMT *stmt = init_and_prepare_stmt("call AggiungiNuovoTavolo(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_inout_param_smallint(0, &numero_tavolo, params);
	set_inout_param_tinyint(1, &max_commensali, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
	return TRUE;
}

mybool manager_aggiungi_nuovo_ingrediente() {
	char nome[21] = { 0 };
	int disp_iniziale;
	double costo_al_kg;

	form_field fields[3];
	string_form_field(fields, 0, "Nome", 1, 20, 20, nome);
	int_form_field(fields, 1, "Disponibilità iniziale", 1, 19, &disp_iniziale);
	double_form_field(fields, 2, "Costo al kg", 1, 10, &costo_al_kg);

	checked_show_form(fields, 3);

    MYSQL_STMT *stmt = init_and_prepare_stmt("call AggiungiNuovoIngrediente(?,?,?)");
    INIT_MYSQL_BIND(params, 3);
    set_in_param_string(0, nome, params);
    set_inout_param_int(1, &disp_iniziale, params);
	set_inout_param_double(2, &costo_al_kg, params);
    bind_param_stmt(stmt, params);

    checked_execute_stmt(stmt);

    close_everything_stmt(stmt);
    return TRUE;
}

mybool manager_aggiungi_prodotto_del_menu() {
	char nome[21] = { 0 };
	double costo_unitario;
	mybool is_bar_menu;
	mybool is_alcolico;

	form_field fields[3];
	string_form_field(fields, 0, "Nome", 1, 20, 20, nome);
	double_form_field(fields, 1, "Costo unitario", 1, 10, &costo_unitario);
	mybool_form_field(fields, 2, "Fa parte del menu bar?", &is_bar_menu);
	checked_show_form(fields, 3);

	if(is_bar_menu) {
		form_field fields_alcolico[1];
		mybool_form_field(fields_alcolico, 0, "Alcolico?", &is_alcolico);
		checked_show_form(fields_alcolico, 1);
	}

    MYSQL_STMT *stmt = init_and_prepare_stmt("call AggiungiProdottoNelMenu(?,?,?,?)");
	INIT_MYSQL_BIND(params, 4);
	set_in_param_string(0, nome, params);
	set_inout_param_double(1, &costo_unitario, params);
	set_inout_param_mybool(2, &is_bar_menu, params);

	if(is_bar_menu) {
		set_inout_param_mybool(3, &is_alcolico, params);
	} else {
		set_in_param_null(3, params);
	}

	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
    return TRUE;
}

mybool manager_associa_prodotto_e_ingrediente() {
	char nome_prodotto[21] = { 0 };
	char nome_ingrediente[21] = { 0 };

	form_field fields[2];
	string_form_field(fields, 0, "Nome del prodotto", 1, 20, 20, nome_prodotto);
    string_form_field(fields, 1, "Nome dell'ingrediente", 1, 20, 20, nome_ingrediente);
	checked_show_form(fields, 2);

	MYSQL_STMT* stmt = init_and_prepare_stmt("call AssociaProdottoAIngrediente(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_in_param_string(0, nome_prodotto, params);
	set_in_param_string(1, nome_ingrediente, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
	return TRUE;
}

mybool manager_crea_turno() {
	turno t;

	form_field fields[10];
	int_form_field(fields, 0, "Giorno inizio", 1, 2, &t.inizio.giorno);
    int_form_field(fields, 1, "Mese inizio", 1, 2, &t.inizio.mese);
    int_form_field(fields, 2, "Anno inizio", 4, 4, &t.inizio.anno);
    int_form_field(fields, 3, "Ora inizio", 1, 2, &t.inizio.ora);
    int_form_field(fields, 4, "Minuto inizio", 1, 2, &t.inizio.minuto);

    int_form_field(fields, 5, "Giorno fine", 1, 2, &t.fine.giorno);
    int_form_field(fields, 6, "Mese fine", 1, 2, &t.fine.mese);
    int_form_field(fields, 7, "Anno fine", 4, 4, &t.fine.anno);
    int_form_field(fields, 8, "Ora fine", 1, 2, &t.fine.ora);
    int_form_field(fields, 9, "Minuto fine", 1, 2, &t.fine.minuto);

	checked_show_form(fields, 10);

	checked_date_check(t.inizio.giorno, t.inizio.mese, t.inizio.anno);
    checked_date_check(t.fine.giorno, t.fine.mese, t.fine.anno);

	INIT_MYSQL_TIME_DATETIME(start_dt, 
		t.inizio.giorno, t.inizio.mese, 
		t.inizio.anno, t.inizio.ora, 
		t.inizio.minuto, 0);

    INIT_MYSQL_TIME_DATETIME(end_dt, 
		t.fine.giorno, t.fine.mese,
        t.fine.anno, t.fine.ora, 
		t.fine.minuto, 0);

	MYSQL_STMT *stmt = init_and_prepare_stmt("call AggiungiTurno(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_inout_param_datetime(0, &start_dt, params);
	set_inout_param_datetime(1, &end_dt, params);
	bind_param_stmt(stmt, params);

    checked_execute_stmt(stmt);

    close_everything_stmt(stmt);
    return TRUE;
}

mybool manager_rimuovi_prodotto_del_menu() {
	char nome_prodotto[21] = { 0 };

	form_field fields[1];
	string_form_field(fields, 0, "Nome del prodotto", 1, 20, 20, nome_prodotto);
	checked_show_form(fields, 1);

	MYSQL_STMT *stmt = init_and_prepare_stmt("call RimuoviProdottoNelMenu(?)");
	INIT_MYSQL_BIND(params, 1);
	set_in_param_string(0, nome_prodotto, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, "il prodotto \"%s\" non esiste.\n", nome_prodotto);

	close_everything_stmt(stmt);
	return is_ok;
}

mybool manager_rimuovi_ingrediente() {
  	char nome_ingrediente[21] = {0};

  	form_field fields[1];
  	string_form_field(fields, 0, "Nome dell'ingrediente", 1, 20, 20, nome_ingrediente);
  	checked_show_form(fields, 1);

  	MYSQL_STMT *stmt = init_and_prepare_stmt("call RimuoviIngrediente(?)");
  	INIT_MYSQL_BIND(params, 1);
  	set_in_param_string(0, nome_ingrediente, params);
  	bind_param_stmt(stmt, params);

  	checked_execute_stmt(stmt);
	
	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, "l'ingrediente \"%s\" non esiste.\n", nome_ingrediente);

    close_everything_stmt(stmt);
    return is_ok;
}

mybool manager_rimuovi_prodotto_e_ingrediente() {
	char nome_prodotto[21] = { 0 };
	char nome_ingrediente[21] = { 0 };
	
	form_field fields[2];
	string_form_field(fields, 0, "Nome del prodotto", 1, 20, 20, nome_prodotto);
	string_form_field(fields, 1, "Nome dell'ingrediente", 1, 20, 20, nome_ingrediente);
	checked_show_form(fields, 2);
	
	MYSQL_STMT *stmt = init_and_prepare_stmt("call RimuoviAssocProdottoEIngrediente(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_in_param_string(0, nome_prodotto, params);
	set_in_param_string(1, nome_ingrediente, params);
	bind_param_stmt(stmt, params);
	
	checked_execute_stmt(stmt);

	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, "l'associazione (\"%s\",\"%s\") non esiste.\n", 
		nome_prodotto, nome_ingrediente);

    close_everything_stmt(stmt);
    return is_ok;
}

static mybool __manager_get_turni(MYSQL_TIME** out_inizio, MYSQL_TIME** out_fine, unsigned long long* n) {
	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniTurni()");
	checked_execute_stmt(stmt);
	
	MYSQL_TIME inizio;
	MYSQL_TIME fine;
	INIT_MYSQL_BIND(params, 2);
	set_inout_param_datetime(0, &inizio, params);
	set_inout_param_datetime(1, &fine, params);
	bind_result_stmt(stmt, params);

	*n = mysql_stmt_num_rows(stmt);
	good_malloc(*out_inizio, MYSQL_TIME, *n);
	good_malloc(*out_fine, MYSQL_TIME, *n);

	begin_fetch_stmt(stmt);
	(*out_inizio)[i] = inizio;
	(*out_fine)[i] = fine;
	end_fetch_stmt();

    close_everything_stmt(stmt);
    return TRUE;
}

mybool manager_visualizza_turni() {
	MYSQL_TIME *turno_inizio = NULL;
	MYSQL_TIME *turno_fine = NULL;
	unsigned long long n_turni;
	if(!__manager_get_turni(&turno_inizio, &turno_fine, &n_turni)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_turni; ++i) {
        printf("inizio: %d/%d/%d %d:%d -- fine: %d/%d/%d %d:%d\n",
                 turno_inizio[i].day, turno_inizio[i].month,
                 turno_inizio[i].year, turno_inizio[i].hour,
                 turno_inizio[i].minute, turno_fine[i].day,
                 turno_fine[i].month, turno_fine[i].year,
                 turno_fine[i].hour, turno_fine[i].minute);
    }

	good_free(turno_inizio);
	good_free(turno_fine);
	return TRUE;
}

static mybool __manager_get_utenti(utente** out_u, unsigned long long *n) {
	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniUtenti()");
	checked_execute_stmt(stmt);

	utente u;
	memset(&u, 0, sizeof(utente));

	INIT_MYSQL_BIND(params, 8);
	set_out_param_string(0, u.username, params);
    set_out_param_string(1, u.nome, params);
    set_out_param_string(2, u.cognome, params);
    set_out_param_string(3, u.cf, params);
    set_out_param_string(4, u.comune_residenza, params);
    set_out_param_string(5, u.comune_nascita, params);
    set_inout_param_date(6, &u.data_nascita, params);
    set_inout_param_int(7, &u.ruolo, params);
	bind_result_stmt(stmt, params);

    *n = mysql_stmt_num_rows(stmt);
    good_malloc(*out_u, utente, *n);

    begin_fetch_stmt(stmt);
	memcpy(&(*out_u)[i], &u, sizeof(utente));
	memset(&u, 0, sizeof(utente));
	end_fetch_stmt();

    close_everything_stmt(stmt);
    return TRUE;
}

mybool manager_visualizza_utenti() {
	unsigned long long n_ut;
	utente *ut = NULL;

	if(!__manager_get_utenti(&ut, &n_ut)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_ut; ++i) {
		printf("\"%s\" [%s]: %s %s residente a: %s, nato a: %s, il: %d/%d/%d (%s)\n",
			ut[i].username, role_to_str(ut[i].ruolo),ut[i].nome, ut[i].cognome, 
			ut[i].comune_residenza,ut[i].comune_nascita, ut[i].data_nascita.day, 
			ut[i].data_nascita.month, ut[i].data_nascita.year, ut[i].cf);
	}

	good_free(ut);
	return TRUE;
}

static mybool __manager_get_tavoli(tavolo **out_t, unsigned long long *n) {
	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniTavoli()");
	checked_execute_stmt(stmt);

	tavolo t;
	
	INIT_MYSQL_BIND(params, 4);
	set_inout_param_int(0, &t.num_tavolo, params);
	set_inout_param_int(1, &t.num_max_commensali, params);
	set_inout_param_int(2, &t.is_occupato, params); /*MYBOOL*/
	set_inout_param_int(3, &t.is_attivo, params); /*MYBOOL*/
	bind_result_stmt(stmt, params);

	*n = mysql_stmt_num_rows(stmt);
	good_malloc(*out_t , tavolo, *n);

	begin_fetch_stmt(stmt);
	memcpy(&(*out_t)[i], &t, sizeof(tavolo));
	end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}

mybool manager_visualizza_tavoli() {
	unsigned long long n_t;
	tavolo *t = NULL;

	if(!__manager_get_tavoli(&t, &n_t)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_t; ++i) {
		printf("tavolo: %d, max comm: %d, occupato: %s, attivo: %s\n",
			t[i].num_tavolo, t[i].num_max_commensali, 
			mybool_to_str(t[i].is_occupato), mybool_to_str(t[i].is_attivo));
	}

	good_free(t);
	return TRUE;
}

static mybool __manager_assegna_turno_perform(tavolo* ta, MYSQL_TIME* tu, utente *u) {
	MYSQL_STMT *stmt = init_and_prepare_stmt("call AssegnaTurno(?,?,?)");

	INIT_MYSQL_BIND(params, 3);
	set_inout_param_smallint(0, &(ta->num_tavolo), params);
	set_inout_param_datetime(1, &tu, params);
	set_in_param_string(2, u->username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
	return TRUE;
}

static void __manager_assegna_turno_print(tavolo *ta, 
	MYSQL_TIME *tu_inizio, MYSQL_TIME *tu_fine, 
	utente *u, unsigned long long n_ta, unsigned long long n_tu, 
	unsigned long long n_u) {

	puts("TAVOLI");
	for(unsigned long long i = 0; i < n_ta; ++i) {
		printf("%lld) tavolo: %d, max commensali: %d\n", 
			i + 1, ta[i].num_tavolo, ta[i].num_max_commensali);
	}

	puts("\nUTENTI");
	for(unsigned long long i = 0; i < n_u; ++i) {
		printf("%lld) %s %s (%s)\n", 
			i + 1, u[i].nome, u[i].cognome, 
			role_to_str(u[i].ruolo));
	}

	puts("\nTURNI");
	for(unsigned long long i = 0; i < n_tu; ++i) {
		printf("%lld) inizio: %d/%d/%d %d:%d -- fine: %d/%d/%d %d:%d\n", 
			i + 1, tu_inizio[i].day, tu_inizio[i].month, 
			tu_inizio[i].year, tu_inizio[i].hour, tu_inizio[i].minute,
			tu_fine[i].day, tu_fine[i].month, tu_fine[i].year, 
			tu_fine[i].hour, tu_fine[i].minute);
	}
}

typedef struct {
	int idx_cameriere;
	int idx_tavolo;
	int idx_turno;
} __assegna_turno_choice;

static mybool __manager_assegna_turno_readchoice(__assegna_turno_choice* choice) {
	form_field fields[3];
	int_form_field(fields, 0, "Tavolo", 1, 5, &(choice->idx_tavolo));
	int_form_field(fields, 1, "Cameriere", 1, 19, &(choice->idx_cameriere));
	int_form_field(fields, 2, "Turno", 1, 19, &(choice->idx_turno));

	checked_show_form(fields, 3);

	return TRUE;
}

mybool manager_assegna_turno() {
	tavolo *ta = NULL;
	utente *u = NULL;
	MYSQL_TIME *tu_inizio = NULL;
	MYSQL_TIME *tu_fine = NULL;

	unsigned long long n_ta;
	unsigned long long n_u;
	unsigned long long n_tu;

	if(!__manager_get_tavoli(&ta, &n_ta)) {
		return FALSE;
	}

	if(!__manager_get_utenti(&u, &n_u)) {
		good_free(ta);
		return FALSE;
	}
	
	if(!__manager_get_turni(&tu_inizio, &tu_fine, &n_tu)) {
		good_free(ta);
		good_free(u);
		return FALSE;
	}

	__manager_assegna_turno_print(ta, tu_inizio, tu_fine, u, n_ta, n_tu, n_u);

	mybool is_ok = FALSE;

	__assegna_turno_choice choice;
	if((is_ok = __manager_assegna_turno_readchoice(&choice))) {
		is_ok = __manager_assegna_turno_perform(
			&(ta[choice.idx_tavolo - 1]), 
			&(tu_inizio[choice.idx_turno - 1]), 
			&(u[choice.idx_cameriere - 1]));
	}

	good_free(ta);
	good_free(u);
	good_free(tu_inizio);
	good_free(tu_fine);
	return is_ok;
}