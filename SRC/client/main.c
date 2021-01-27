#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/limits.h>

#include "parse_dbms_conn_config.h"
#include "mysql_utils.h"
#include "global_config.h"
#include "macros.h"

#ifndef USERS_DIR_DFL
#define USERS_DIR_DFL "./users"
#endif

#define OPT_WITH_ARG(arg, set) \
	if(strcmp(argv[i], arg) == 0) { \
		int i_plus_one = i + 1; \
		if(i_plus_one == argc) { \
			fprintf(stderr, "errore: %s richiede un argomento\n", arg); \
			print_help_and_exit(EXIT_FAILURE, "usage corretto"); \
		} \
		set = argv[i + 1]; \
		i = i_plus_one; \
		continue; \
	}

#define OPT_HELP_AND_EXIT() \
	if(strcmp(argv[i], "--help") == 0) { \
		print_help_and_exit(EXIT_SUCCESS, "help"); \
	}

config cfg;

mybool attempt_login(const char* password, const char* users_dir);

void print_help_and_exit(int code, const char* msg) {
	FILE* f = stdout;

	if(code != EXIT_SUCCESS) {
		f = stderr;
	}

	fprintf(f, 
		"%s\n\t--username <username>\n\t--password <password>\n\t[--users <users_dir>]\n\t[--help]\n", 
		msg);

	exit(code);
}

void close_and_exit(int code) {
	mysql_close(cfg.db_conn);
	exit(code);
}

void handler_close_before_exit() {
	close_and_exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)  {
	cfg.username = NULL;
	char* password = NULL;
	char* users_dir = USERS_DIR_DFL;

	for(int i = 1; i < argc; ++i) {
		OPT_HELP_AND_EXIT();
		OPT_WITH_ARG("--username", cfg.username);
		OPT_WITH_ARG("--password", password);
		OPT_WITH_ARG("--users", users_dir);
		printf("warning -- %s: opzione sconosciuta\n", argv[i]);
	}

	if(cfg.username == NULL || password == NULL) {
		print_help_and_exit(EXIT_FAILURE, "username e password devono essere specificati");
	}

	char json_file_path[PATH_MAX + 2] = { 0 };
	snprintf(json_file_path, PATH_MAX + 1, "%s/%s", users_dir, LOGIN_JSON_FILE);

	dbms_conn_config dbms_conf;
	if(parse_dbms_conn_config(json_file_path, &dbms_conf) == FALSE) {
		printf("impossibile parsare il file json (%s)\n", json_file_path);
		exit(EXIT_FAILURE);
	}

	if(dbms_conf.db_port < 0 || dbms_conf.db_port > 65535) {
		puts("port range invalido (valido: 0 <= port <= 65535)");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);

	if((cfg.db_conn = mysql_init(NULL)) == NULL) {
		puts("impossibile iniziallizzare la libreria mysql (mysql_init)");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, (void (*)(int)) handler_close_before_exit);
	signal(SIGTERM, (void (*)(int)) handler_close_before_exit);

	printf("tentativo di connessione: %s in %s@%s:%d...\n", 
		dbms_conf.db_name, 
		dbms_conf.db_username, 
		dbms_conf.db_hostname, 
		dbms_conf.db_port);

	if(mysql_real_connect(cfg.db_conn, 
						dbms_conf.db_hostname, 
						dbms_conf.db_username, 
						dbms_conf.db_password, 
						dbms_conf.db_name, 
						dbms_conf.db_port, 
						NULL, 
						CLIENT_MULTI_RESULTS | CLIENT_MULTI_STATEMENTS) == NULL) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_real_connect");
	}

	if(mysql_autocommit(cfg.db_conn, FALSE)) {
		MYSQL_BASIC_PRINTERROR_EXIT("mysql_autocommit");
	}

	if(attempt_login(password, users_dir) == FALSE) {
		puts("credenziali per l'accesso al sistema non valide");
		close_and_exit(EXIT_FAILURE);
	}

	while(show_menu());

	close_and_exit(EXIT_SUCCESS);
}