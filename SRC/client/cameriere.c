#include "op.h"
#include "mysql_utils.h"
#include "global_config.h"
#include "goodmalloc.h"

typedef struct {
	int num_tavolo;
	mybool is_occupato;
	MYSQL_TIME data_ora_occupazione;
	int num_commensali;
	mybool is_servito_almeno_una_volta;
} situazione_tavolo;

typedef struct {
	char nome_prod[21];
	int num_ord_per_tavolo;
	int num_sc_per_ord;
} scelta_del_cliente;

typedef struct {
	char nome_prod[21];
	MYSQL_TIME data_ora_occ;
	int num_ord_per_tavolo;
	int num_sc_per_ord;
	int num_t;
} scelta_del_cliente_espletata;

static mybool checked_show_form_action(form_field* fields, int nf) {
	checked_show_form(fields, nf);
	return TRUE;
}

static mybool checked_execute_stmt_action(MYSQL_STMT* stmt) {
	checked_execute_stmt(stmt);
	return TRUE;
}

static mybool __cameriere_agg_ing_extra_alla_scelta_perform(
	MYSQL_TIME* data_ora_occ, int num_ord_per_tav, int num_sc_per_ord,
	const char *ing, double qt_gr) {
	
	char nome_ing[21] = { 0 };
	memcpy(nome_ing, ing , 20);

	MYSQL_STMT *stmt = init_and_prepare_stmt("call AggiungiIngExtraAllaScelta(?,?,?,?,?,?)");

	INIT_MYSQL_BIND(params, 6);
	set_inout_param_datetime(0, data_ora_occ, params);
	set_inout_param_int(1, &num_ord_per_tav, params);
	set_inout_param_int(2, &num_sc_per_ord, params);
	set_in_param_string(3, nome_ing, params);
	set_inout_param_double(4, &qt_gr, params);
	set_in_param_string(5, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	close_everything_stmt(stmt);
	return TRUE;
}

static mybool __cameriere_get_situazione_tavolo(
		situazione_tavolo** st, unsigned long long *n_st) {

	*n_st = 0;

	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniTavoliDiCompetenza(?)");

	INIT_MYSQL_BIND(params, 1);
	set_in_param_string(0, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	*n_st = mysql_stmt_num_rows(stmt);
	good_malloc(*st, situazione_tavolo, *n_st);

	situazione_tavolo myst;
	memset(&myst.data_ora_occupazione, 0, sizeof(myst.data_ora_occupazione));

	INIT_MYSQL_BIND(res, 5);
	set_inout_param_int(0, &myst.num_tavolo, res);
	set_inout_param_int(1, &myst.is_occupato, res);
	set_inout_param_datetime(2, &myst.data_ora_occupazione, res);
	set_inout_param_int(3, &myst.num_commensali, res);
	set_inout_param_int(4, &myst.is_servito_almeno_una_volta, res);
	bind_result_stmt(stmt, res);

	begin_fetch_stmt(stmt);
	memcpy(&(*st)[i], &myst, sizeof(myst));
    memset(&myst.data_ora_occupazione, 0, sizeof(myst.data_ora_occupazione));
    end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}

static mybool __cameriere_prendi_ordinazione_common(mybool close) {
	situazione_tavolo *st = NULL;

	unsigned long long n_st;
	
	if (!__cameriere_get_situazione_tavolo(&st, &n_st)) {
		return FALSE;
	}
	
	for (unsigned long long i = 0; i < n_st; ++i) {
		printf("(%llu) --> tavolo: %d, occupato: %s", i + 1,
			st[i].num_tavolo,
			mybool_to_str(st[i].is_occupato));
		
		if (st[i].is_occupato) {
			printf(", commensali: %d, servito: %s\n", st[i].num_commensali,
				mybool_to_str(st[i].is_servito_almeno_una_volta));
		} else {
			puts("");
		}
	}

	unsigned long long opt = 0;

	form_field fields[1];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	if(!checked_show_form_action(fields, 1)) {
		good_free(st);
		return FALSE;
	}

	if(opt < 1 || opt > n_st) {
		puts("scelta non valida");
		good_free(st);
		return FALSE;
	}

	MYSQL_STMT *stmt = init_and_prepare_stmt(
		close ? "call ChiudiOrdinazione(?,?)" : "call PrendiOrdinazione(?,?)");

	INIT_MYSQL_BIND(params, 2);
	set_inout_param_datetime(0, &st[opt - 1].data_ora_occupazione, params);
	set_in_param_string(1, cfg.username, params);
	bind_param_stmt(stmt, params);
	if(!checked_execute_stmt_action(stmt)) {
		good_free(st);
		return FALSE;
	}

	mybool is_ok = TRUE;

	if(close) {
		if(mysql_stmt_affected_rows(stmt) == 0) {
			printf("Impossibile chiudere l'ordinazione (opt: %llu)\n", 
					opt);
			is_ok = FALSE;
		}
	}

	good_free(st);
	close_everything_stmt(stmt);
	return is_ok;
}

static mybool __cameriere_get_scelte_del_cliente(
		MYSQL_TIME* data_ora_occ, scelta_del_cliente** sdc, 
		unsigned long long *n_sdc) {

	*n_sdc = 0;

	MYSQL_STMT* stmt = init_and_prepare_stmt("call OttieniSceltePerOrdinazione(?,?)");
	INIT_MYSQL_BIND(params, 2);
	set_inout_param_datetime(0, data_ora_occ, params);
	set_in_param_string(1, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	*n_sdc = mysql_stmt_num_rows(stmt);
	good_malloc(*sdc, scelta_del_cliente, *n_sdc);

	scelta_del_cliente base;
	memset(&base, 0, sizeof(base));

	INIT_MYSQL_BIND(res_params, 3);
	set_inout_param_int(0, &base.num_ord_per_tavolo, res_params);
	set_inout_param_int(1, &base.num_sc_per_ord, res_params);
	set_out_param_string(2, base.nome_prod, res_params);
	bind_result_stmt(stmt, res_params);

	begin_fetch_stmt(stmt);
	memcpy(&(*sdc)[i], &base, sizeof(base));
	memset(&base, 0, sizeof(base));
	end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}

static mybool __cameriere_get_scelte_espletate(
		scelta_del_cliente_espletata** esp, 
		unsigned long long *n_esp) {
	
	*n_esp = 0;

	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniScelteEspletate(?)");
	INIT_MYSQL_BIND(params, 1);
	set_in_param_string(0, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	*n_esp = mysql_stmt_num_rows(stmt);
	good_malloc(*esp, scelta_del_cliente_espletata, *n_esp);

	scelta_del_cliente_espletata sdc_esp;
	memset(&sdc_esp, 0, sizeof(sdc_esp));

	INIT_MYSQL_BIND(resp, 5);
	set_inout_param_datetime(0, &sdc_esp.data_ora_occ, resp);
	set_inout_param_int(1, &sdc_esp.num_ord_per_tavolo, resp);
	set_inout_param_int(2, &sdc_esp.num_sc_per_ord, resp);
	set_inout_param_int(3, &sdc_esp.num_t, resp);
	set_out_param_string(4, sdc_esp.nome_prod, resp);
	bind_result_stmt(stmt, resp);

	begin_fetch_stmt(stmt);
	memcpy(&(*esp)[i], &sdc_esp, sizeof(sdc_esp));
	memset(&sdc_esp, 0, sizeof(sdc_esp));
	end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}

static mybool __cameriere_effettua_consegna_perform(
	scelta_del_cliente_espletata* esp, unsigned long long opt) {

	MYSQL_STMT *stmt = init_and_prepare_stmt("call EffettuaConsegna(?,?,?,?)");
	INIT_MYSQL_BIND(params, 4);
	set_inout_param_datetime(0, &(esp->data_ora_occ), params);
	set_inout_param_int(1, &(esp->num_ord_per_tavolo), params);
	set_inout_param_int(2, &(esp->num_sc_per_ord), params);
	set_in_param_string(3, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, "Impossibile effettuare la consegna (opt: %llu)\n", 
							opt);

	close_everything_stmt(stmt);
	return is_ok;
}

mybool cameriere_visualizza_situazione_tavoli() {
	situazione_tavolo *st = NULL;
	unsigned long long n_st;

	if(!__cameriere_get_situazione_tavolo(&st, &n_st)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_st; ++i) {
		printf("--> tavolo: %d, occupato: %s", 
			st[i].num_tavolo, 
			mybool_to_str(st[i].is_occupato));
		if(st[i].is_occupato) {
			printf(", commensali: %d, servito: %s\n", 
				st[i].num_commensali, 
				mybool_to_str(st[i].is_servito_almeno_una_volta));
		} else {
			puts("");
		}
	}

	good_free(st);
	return TRUE;
}

mybool cameriere_chiudi_ordinazione() {
	return __cameriere_prendi_ordinazione_common(TRUE);
}

mybool cameriere_prendi_ordinazione() {
	return __cameriere_prendi_ordinazione_common(FALSE);
}

mybool cameriere_prendi_scelta_per_ordinazione() {
	situazione_tavolo *st = NULL;
	unsigned long long n_st;
	
	if (!__cameriere_get_situazione_tavolo(&st, &n_st)) {
		return FALSE;
	}
	
	for (unsigned long long i = 0; i < n_st; ++i) {
		printf("(%llu) --> tavolo: %d, occupato: %s", i + 1,
			st[i].num_tavolo,
			mybool_to_str(st[i].is_occupato));
		
		if (st[i].is_occupato) {
			printf(", commensali: %d, servito: %s\n", st[i].num_commensali,
				mybool_to_str(st[i].is_servito_almeno_una_volta));
		} else {
			puts("");
		}
	}

	unsigned long long opt = 0;
	char nome_prod[21] = { 0 };

	form_field fields[2];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	string_form_field(fields, 1, "Prodotto", 1, 20, 20, nome_prod);
	if(!checked_show_form_action(fields, 2)) {
		good_free(st);
		return FALSE;
	}

	if(opt < 1 || opt > n_st) {
		puts("scelta non valida");
		good_free(st);
		return FALSE;
	}

	MYSQL_STMT *stmt = init_and_prepare_stmt("call PrendiSceltaPerOrd(?,?,?)");
	INIT_MYSQL_BIND(params, 3);
	set_inout_param_datetime(0, &st[opt - 1].data_ora_occupazione, params);
	set_in_param_string(1, nome_prod, params);
	set_in_param_string(2, cfg.username, params);
	bind_param_stmt(stmt, params);

	if(!checked_execute_stmt_action(stmt)) {
		good_free(st);
		return FALSE;
	}

	good_free(st);
	return TRUE;
}

mybool cameriere_aggiungi_ing_extra_per_scelta() {
	situazione_tavolo *st = NULL;
	unsigned long long n_st;
	
	if (!__cameriere_get_situazione_tavolo(&st, &n_st)) {
		return FALSE;
	}
	
	for (unsigned long long i = 0; i < n_st; ++i) {
		printf("(%llu) --> tavolo: %d, occupato: %s", i + 1,
			st[i].num_tavolo,
			mybool_to_str(st[i].is_occupato));
		
		if (st[i].is_occupato) {
			printf(", commensali: %d, servito: %s\n", st[i].num_commensali,
				mybool_to_str(st[i].is_servito_almeno_una_volta));
		} else {
			puts("");
		}
	}

	unsigned long long opt = 0;

	form_field fields[1];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	if(!checked_show_form_action(fields, 1)) {
		good_free(st);
		return FALSE;
	}

	if(opt < 1 || opt > n_st) {
		puts("scelta non valida");
		good_free(st);
		return FALSE;
	}

	unsigned long long opt_minus_one = opt - 1;
	
	scelta_del_cliente* sdc = NULL;
	unsigned long long n_sdc;
	if(__cameriere_get_scelte_del_cliente(
		&st[opt_minus_one].data_ora_occupazione, &sdc, &n_sdc) == FALSE) {
		
		good_free(st);
		return FALSE;
	}

	if(n_sdc > 0) {
		printf("--> Ordinazione %d\n", sdc[0].num_ord_per_tavolo);
		for(unsigned long long i = 0; i < n_sdc; ++i) {
			printf("\t** SCELTA %d **\n\t++ Prodotto: %s\n",
				sdc[i].num_sc_per_ord, sdc[i].nome_prod);
   	 	}
	}

	double q_gr;
	char nome_ing[21] = { 0 };
	unsigned long long opt_1 = 0;

	form_field fields_1[3];
	int_form_field(fields_1, 0, "Opzione", 1, 19, &opt_1);
	string_form_field(fields_1, 1, "Ingrediente", 1, 20, 20, nome_ing);
	double_form_field(fields_1, 2, "Quantita (gr)", 1, 10, &q_gr);
	if(!checked_show_form_action(fields_1, 3)) {
		good_free(st);
		good_free(sdc);
		return FALSE;
	}

	if(opt_1 < 1 || opt_1 > n_sdc){
		puts("scelta non valida");
		good_free(st);
		good_free(sdc);
		return FALSE;
	}

	unsigned long long opt_1_minus_one = opt_1 - 1;

	mybool is_ok = __cameriere_agg_ing_extra_alla_scelta_perform(
		&st[opt_minus_one].data_ora_occupazione, 
		sdc[opt_1_minus_one].num_ord_per_tavolo,
		sdc[opt_1_minus_one].num_sc_per_ord, nome_ing, q_gr);

	good_free(st);
	good_free(sdc);
	return is_ok;
}

mybool cameriere_visualizza_scelte_espletate() {
	scelta_del_cliente_espletata* esp = NULL;
	unsigned long long n_esp;

	if(__cameriere_get_scelte_espletate(&esp, &n_esp) == FALSE) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_esp; ++i) {
		printf("Tavolo %d, # ord: %d, # scelta: %d, prodotto: %s\n", 
			esp[i].num_t, esp[i].num_ord_per_tavolo, esp[i].num_sc_per_ord, 
			esp[i].nome_prod);
	}

	good_free(esp);
	return TRUE;
}

mybool cameriere_effettua_consegna() {
	scelta_del_cliente_espletata* esp = NULL;
	unsigned long long n_esp;

	if(__cameriere_get_scelte_espletate(&esp, &n_esp) == FALSE) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_esp; ++i) {
		printf("(%llu) Tavolo %d, # ord: %d, # scelta: %d, prodotto: %s\n",
			i + 1, esp[i].num_t, esp[i].num_ord_per_tavolo, 
			esp[i].num_sc_per_ord, esp[i].nome_prod);
	}

	unsigned long long opt = 0;

	form_field fields[1];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	if(!checked_show_form_action(fields, 1)) {
		good_free(esp);
		return FALSE;
	}

	if(opt < 1 || opt > n_esp) {
		puts("scelta non valida");
		good_free(esp);
		return FALSE;
	}

	mybool is_ok = __cameriere_effettua_consegna_perform(esp, opt);

	good_free(esp);
	return is_ok;
}

mybool cameriere_visualizza_tavoli_assegnati() {
	MYSQL_STMT *stmt = init_and_prepare_stmt("call OttieniTavoliAssegnati(?)");
	INIT_MYSQL_BIND(params, 1);
	set_in_param_string(0, cfg.username, params);
	bind_param_stmt(stmt, params);

	checked_execute_stmt(stmt);

	int num_t;

	RESET_MYSQL_BIND(params);
	set_inout_param_int(0, &num_t, params);
	bind_result_stmt(stmt, params);

	begin_fetch_stmt(stmt);
	printf("%d\n", num_t);
	end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}