#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "interactive.h"

boolean show_menu(menu_option* opts, int opts_len, char* username, MYSQL* conn, char* menu_name) {
	int i;

	printf("\n\n---%s---\n", menu_name);
	
	for(i = 0; i < opts_len; ++i) {
		printf("(%d) %s\n", i + 1, opts[i].option);
	}

	int go_back_option = i + 1;
	printf("(%d) Esci dal programma\n", go_back_option);
	printf("\n[%s]> ", username);

	char *endptr = NULL;
	char buf[1024] = { 0 };
	
	fgets(buf, 1024, stdin);
	
	errno = 0;
	int opt = strtol(buf, &endptr, 10);

	if(opt == go_back_option) {
		return TRUE;
	}

	if(opt < 1 || opt > go_back_option) {
		printf("Opzione invalida: %d\n", opt);
	} else {
		opts[opt - 1].handler(username, conn);
	}

	return FALSE;
}
