#include "op.h"
#include "global_config.h"
#include "goodmalloc.h"
#include "mysql_utils.h"

typedef struct {
	char nome_prod[21];
	MYSQL_TIME tavolo_occupato;
	int num_tavolo;
	int num_ord_per_tavolo;
	int num_sc_per_ord;
} scelta_da_preparare;

static mybool checked_show_form_action(form_field* fields, int nf) {
	checked_show_form(fields, nf);
	return TRUE;
}

static mybool checked_execute_stmt_action(MYSQL_STMT* stmt) {
	checked_execute_stmt(stmt);
	return TRUE;
}

static mybool __lavoratore_cucina_get_scelte_da_preparare(
		mybool da_esp, scelta_da_preparare **sdp_out, unsigned long long *n_sdp_out) {

	MYSQL_STMT* stmt = init_and_prepare_stmt(
		da_esp ? "call OttieniSceltePreseInCaricoNonEspletate(?)" : "call OttieniScelteDaPreparare()");
    
	INIT_MYSQL_BIND(esp_params, 1);

	if(da_esp) {
		set_in_param_string(0, cfg.username, esp_params);
		bind_param_stmt(stmt, esp_params);
	}

	checked_execute_stmt(stmt);

	*n_sdp_out = mysql_stmt_num_rows(stmt);
	good_malloc(*sdp_out, scelta_da_preparare, *n_sdp_out);

	scelta_da_preparare sdp;
	memset(&sdp, 0, sizeof(sdp));

	INIT_MYSQL_BIND(params, 5);
	set_inout_param_datetime(0, &sdp.tavolo_occupato, params);
	set_inout_param_int(1, &sdp.num_tavolo, params);
	set_inout_param_int(2, &sdp.num_ord_per_tavolo, params);
	set_inout_param_int(3, &sdp.num_sc_per_ord, params);
	set_out_param_string(4, sdp.nome_prod, params);
	bind_result_stmt(stmt, params);

	begin_fetch_stmt(stmt);
	memcpy(sdp_out[i], &sdp, sizeof(sdp));
	memset(&sdp, 0, sizeof(sdp));
	end_fetch_stmt();

	close_everything_stmt(stmt);
	return TRUE;
}

static mybool __lavoratore_cucina_visualizza_scelte_da_preparare_oresp_common(mybool esp) {
	scelta_da_preparare *sdp = NULL;
	unsigned long long n_sdp;
	if(!__lavoratore_cucina_get_scelte_da_preparare(esp, &sdp, &n_sdp)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_sdp; ++i) {
		printf("--> Tavolo: %d, # ord: %d, # sc per ord: %d, prodotto: %s\n", 
			sdp[i].num_tavolo, sdp[i].num_ord_per_tavolo, sdp[i].num_sc_per_ord, 
			sdp[i].nome_prod);
	}

	good_free(sdp);
	return TRUE;
}

mybool lavoratore_cucina_prendi_in_carico_scelta_da_preparare() {
	scelta_da_preparare *sdp = NULL;
	unsigned long long n_sdp;
	if(!__lavoratore_cucina_get_scelte_da_preparare(FALSE, &sdp, &n_sdp)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_sdp; ++i) {
		printf("(%llu) Tavolo: %d, # ord: %d, # sc per ord: %d, prodotto: %s\n",
			i + 1, sdp[i].num_tavolo, sdp[i].num_ord_per_tavolo, 
			sdp[i].num_sc_per_ord,  sdp[i].nome_prod);
	}
	
	unsigned long long opt;

	form_field fields[1];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	if(!checked_show_form_action(fields, 1)) {
		good_free(sdp);
		return FALSE;
	}

	if(opt < 1 || opt > n_sdp) {
		puts("opzione non valida");
		good_free(sdp);
		return FALSE;
	}

	int opt_minus_one = opt - 1;

	MYSQL_STMT *stmt = init_and_prepare_stmt("call PrendiInCaricoScelta(?,?,?,?,?)");
	INIT_MYSQL_BIND(params, 5);
	set_inout_param_datetime(0, &sdp[opt_minus_one].tavolo_occupato, params);
	set_inout_param_int(1, &sdp[opt_minus_one].num_ord_per_tavolo, params);
	set_inout_param_int(2, &sdp[opt_minus_one].num_sc_per_ord, params);
	set_in_param_string(3, sdp[opt_minus_one].nome_prod, params);
	set_in_param_string(4, cfg.username, params);
	bind_param_stmt(stmt, params);

	if(!checked_execute_stmt_action(stmt)) {
		good_free(sdp);
		return FALSE;
	}

	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, 
		"non è stato possibile prendere in carico la scelta (opt: %llu)\n", 
		opt);

	good_free(sdp);
	close_everything_stmt(stmt);
	return is_ok; 
}

mybool lavoratore_cucina_espleta_scelta() {
	scelta_da_preparare *sdp = NULL;
	unsigned long long n_sdp;
	if(!__lavoratore_cucina_get_scelte_da_preparare(TRUE, &sdp, &n_sdp)) {
		return FALSE;
	}

	for(unsigned long long i = 0; i < n_sdp; ++i) {
		printf("(%llu) Tavolo: %d, # ord: %d, # sc per ord: %d, prodotto: %s\n",
			i + 1, sdp[i].num_tavolo, sdp[i].num_ord_per_tavolo, 
			sdp[i].num_sc_per_ord,  sdp[i].nome_prod);
	}
	
	unsigned long long opt;

	form_field fields[1];
	int_form_field(fields, 0, "Opzione", 1, 19, &opt);
	if(!checked_show_form_action(fields, 1)) {
		good_free(sdp);
		return FALSE;
	}

	if(opt < 1 || opt > n_sdp) {
		puts("opzione non valida");
		good_free(sdp);
		return FALSE;
	}

	int opt_minus_one = opt - 1;

	MYSQL_STMT* stmt = init_and_prepare_stmt("call EspletaScelta(?,?,?,?)");

	INIT_MYSQL_BIND(params, 4);
	set_inout_param_datetime(0, &sdp[opt_minus_one].tavolo_occupato, params);
	set_inout_param_int(1, &sdp[opt_minus_one].num_ord_per_tavolo, params);
	set_inout_param_int(2, &sdp[opt_minus_one].num_sc_per_ord, params);
	set_in_param_string(3, cfg.username, params);
	bind_param_stmt(stmt, params);

	if(!checked_execute_stmt_action(stmt)) {
		good_free(sdp);
		return FALSE;
	}

	mybool is_ok = TRUE;
	check_affected_stmt_rows(is_ok, stmt, 
		"non è stato possibile espletare la scelta (opt: %llu)\n", opt);

	good_free(sdp);
	close_everything_stmt(stmt);
	return is_ok; 
}

mybool lavoretore_cucina_visualizza_info_scelte_prese_in_carico() {
	int num_tavolo;
	int num_ord_per_tavolo;
	int num_sc_per_ord;
	char nome_prod[21] = { 0 };
	char nome_ing[21] = { 0 };
	double qt_in_gr;
	my_bool ing_is_null;

	MYSQL_STMT *stmt =
        init_and_prepare_stmt("call OttieniInfoProdottiDiScelteInCarico(?)");

	INIT_MYSQL_BIND(params_in, 1);
	set_in_param_string(0, cfg.username, params_in);
	bind_param_stmt(stmt, params_in);

	checked_execute_stmt(stmt);

	INIT_MYSQL_BIND(params, 6);
	set_inout_param_int(0, &num_tavolo, params);
	set_inout_param_int(1, &num_ord_per_tavolo, params);
	set_inout_param_int(2, &num_sc_per_ord, params);
	set_out_param_string(3, nome_prod, params);
	set_out_param_maybe_null_string(4, nome_ing, &ing_is_null, params);
	set_inout_param_double(5, &qt_in_gr, params);

	begin_fetch_stmt(stmt);
	printf("--> Tavolo: %d, # ord: %d, # sc per ord: %d\n\t"
			"* Prodotto: %s", num_tavolo, num_ord_per_tavolo,
			num_sc_per_ord, nome_prod);

	if(!ing_is_null) {
		printf("\n\t* Ingrediente extra: %s\n\t* Quantita in gr: %lf gr\n",
				nome_ing, qt_in_gr);
	} else {
		puts("");
	}

	memset(nome_prod, 0, sizeof(nome_prod));
	memset(nome_ing, 0, sizeof(nome_ing));
	end_fetch_stmt();
	
	close_everything_stmt(stmt);
	return TRUE;
}

mybool lavoratore_cucina_visualizza_scelte_da_preparare() {
	return __lavoratore_cucina_visualizza_scelte_da_preparare_oresp_common(FALSE);
}

mybool lavoratore_cucina_visualizza_scelte_presa_in_carico_da_espletare() {
	return __lavoratore_cucina_visualizza_scelte_da_preparare_oresp_common(TRUE);
}