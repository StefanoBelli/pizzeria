#include <stdio.h>
#include <mysql.h>

#include "interactive.h"

boolean show_menu(const menu_option* opts, int opts_len, char* username, MYSQL* conn, char* menu_name) {
	int i;

	printf("\n\n---%s---\n", menu_name);
	
	for(i = 0; i < opts_len; ++i) {
		printf("(%d) %s\n", i + 1, opts[i].option);
	}

	int go_back_option = i + 1;
	printf("(%d) Vai indietro / Esci dal programma\n", go_back_option);
	printf("\n[%s]> ", username);

	int opt;
	scanf("%d", &opt);

	if(opt == go_back_option) {
		return TRUE;
	}

	if(opt < 1 || opt > go_back_option) {
		printf("Opzione invalida: %d\n", opt);
	} else {
		opts[i].handler(username, conn);
	}

	return FALSE;
}
