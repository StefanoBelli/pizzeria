#include <linux/limits.h>

#include "global_config.h"
#include "mybool.h"
#include "menu_entries.h"
#include "mysql_utils.h"
#include "parse_dbms_conn_config.h"
#include "macros.h"
#include "role.h"

static mybool set_menu_based_on_role(role r) {
	if(r == ROLE_UNKNOWN)
		return FALSE;

	if(r == ROLE_MANAGER) {
		cfg.menu_entries = entries_manager;
		cfg.menu_entries_len = ENTRIES_LEN_MANAGER;
    } else if(r == ROLE_CAMERIERE) {
		cfg.menu_entries = entries_cameriere;
		cfg.menu_entries_len = ENTRIES_LEN_CAMERIERE;
    } else if (r == ROLE_BARMAN || r == ROLE_PIZZAIOLO) {
		cfg.menu_entries = entries_barman_e_pizzaiolo;
		cfg.menu_entries_len = ENTRIES_LEN_BARMAN_E_PIZZAIOLO;
    }
	
	return TRUE;
}

static void reparse_and_change_user(role r, const char* users_dir) {
	const char* what;

	if(r == ROLE_MANAGER)
		what = MANAGER_JSON_FILE;
	else if(r == ROLE_BARMAN)
		what = BARMAN_JSON_FILE;
	else if(r == ROLE_PIZZAIOLO)
		what = PIZZAIOLO_JSON_FILE;
	else if(r == ROLE_CAMERIERE)
		what = CAMERIERE_JSON_FILE;

	char json_file_path[PATH_MAX + 2] = {0};
	snprintf(json_file_path, PATH_MAX + 1, "%s/%s", users_dir, what);
	
	dbms_conn_config dbms_conf;
	if (parse_dbms_conn_config(json_file_path, &dbms_conf) == FALSE) {
		printf("impossibile parsare il file json (%s)\n", json_file_path);
		close_and_exit(EXIT_FAILURE);
	}

    if (mysql_change_user(cfg.db_conn, 
							dbms_conf.db_username, 
							dbms_conf.db_password, 
							dbms_conf.db_name)) {
        MYSQL_BASIC_PRINTERROR_EXIT("mysql_change_user");
    }

	free_dbms_conn_config(dbms_conf);
}

mybool attempt_login(const char* password, const char* users_dir) {
	role r = ROLE_UNKNOWN;

	MYSQL_STMT* stmt = init_and_prepare_stmt("call TentaLogin(?,?,?)");

	INIT_MYSQL_BIND(params, 3);
	set_in_param_string(0, cfg.username, params);
	set_in_param_string(1, (char*) password, params);
	set_inout_param_tinyint(2, (int*)&r, params);
	bind_param_stmt(stmt, params);

	if(!execute_stmt(stmt)) {
		close_only_stmt(stmt);
		return FALSE;
	}

	RESET_MYSQL_BIND(params);
	set_inout_param_int(0, (int*)&r, params);
	bind_result_stmt(stmt, params);

	if(mysql_stmt_fetch(stmt)) {
		MYSQL_STMT_BASIC_PRINTERROR_EXIT("mysql_stmt_fetch", stmt);
	}

	close_everything_stmt(stmt);
	
	mybool login = set_menu_based_on_role(r);

	if(login == TRUE) {
		reparse_and_change_user(r, users_dir);
	}

	return login;
}