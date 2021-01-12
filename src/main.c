#ifndef DEFAULT_HOSTNAME
#define DEFAULT_HOSTNAME "localhost"
#endif

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 3306
#endif

#ifndef USEDB
#define USEDB "pizzeriadb"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include <mysql.h>

#include "mystrerror.h"
#include "boolean.h"
#include "interactive.h"
#include "set_main_menu.h"

#define print_help_exit(first) { \
	printf("usage: %s [--no-password] [--hostname HOSTNAME] [--port PORT] [--username USERNAME]\n", first); \
	exit(EXIT_SUCCESS); \
}

#define missing_arg_exit(arg) { \
	printf("%s: missing argument\n", arg); \
	exit(EXIT_FAILURE); \
}

void start_interactive_menu(char* username, MYSQL* conn) {
	int len;
	menu_option* opts = set_main_menu_by_user_role(&len, conn);

	boolean loop = TRUE;
	while(loop) {
		boolean has_to_go_back = show_menu(opts, 
			len, username, conn, "Menu principale");
		if(has_to_go_back == TRUE)
			loop = FALSE;
	}

	FREE_MENU_OPTION(opts, len);
}

int main(int argc, char** argv) {
	char* hostname = DEFAULT_HOSTNAME;
	int port = DEFAULT_PORT;
	char* username = NULL;
	boolean use_password = TRUE;

	for (int i = 1; i < argc; ++i) {
		if(strcmp("--help", argv[i]) == 0) {
			print_help_exit(argv[0]);
		} else if(strcmp("--no-password", argv[i]) == 0) {
			use_password = FALSE;
		} else if(strcmp("--hostname", argv[i]) == 0) {
			if(argv[i + 1] == NULL) {
				missing_arg_exit("--hostname");
			} else {
				hostname = (char*) argv[i + 1];
				++i;
			}
		} else if(strcmp("--port", argv[i]) == 0) {
			if(argv[i + 1] == NULL) {
				missing_arg_exit("--port");
			} else {
				char* endptr = NULL;
				errno = 0;
				port = strtoul(argv[i + 1], &endptr, 10);
				if(endptr != argv[i + 1] + strlen(argv[i + 1])) {
					fputs("unable to convert\n", stderr);
					exit(EXIT_FAILURE);
				}

				if(errno) {
					eprint_exit("strtoul");
				}

				++i;
			}
		} else if(strcmp("--username", argv[i]) == 0) {
			if(argv[i + 1] == NULL) {
				missing_arg_exit("--username");
			} else {
				username = (char*) argv[i + 1];
				++i;
			}
		} else {
			printf("warning - ignoring unknown option: %s\n", argv[i]);
		}
	}

	if(username == NULL) {
		fputs("you must specify username using --username\n", stderr);
		exit(EXIT_FAILURE);
	}

	if(port < 0 || port > 65535) {
		fputs("invalid port range (valid: [0-65535])\n", stderr);
		exit(EXIT_FAILURE);
	}

	MYSQL* conn = mysql_init(NULL);
	if(conn == NULL) {
		mysql_strerror_exit("mysql_init", conn);
	}

	printf("Connecting to: %s:%d as %s...\n", hostname, port, username);

	char pwd[1024];
	if(use_password == TRUE) {
		printf("Using password authentication\nEnter password: ");
		fgets(pwd, 1024, stdin);
		pwd[strlen(pwd) - 1] = 0;
	} else {
		puts("Not using password authentication");
	}

    mysql_optionsv(conn, MYSQL_INIT_COMMAND, 
		"set autocommit = 0",
        "set max_sp_recursion_depth = 255");

    MYSQL* conn_attempt_outcome = 
		mysql_real_connect(conn, hostname, username, pwd, USEDB, port, NULL, 0);

	if(conn_attempt_outcome == NULL) {
		mysql_strerror_exit("mysql_real_connect", conn);
	}

	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	
	start_interactive_menu(username, conn);

	puts("exiting...");

	mysql_close(conn);
	exit(EXIT_SUCCESS);
}
