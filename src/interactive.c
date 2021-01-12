#include <mysql.h>

#include "interactive.h"
#include "boolean.h"

typedef void (*option_handler_fpt)(MYSQL* conn);

typedef struct {
	char* option;
	option_handler_fpt handler;
} menu_option;

void start_interactive_menu(char* username, MYSQL* conn) {
	menu_option opts;
	//sql call GetUserRole
	
	while(TRUE) {
		boolean has_to_go_back = show_menu(&opts, conn);
		if(has_to_go_back == TRUE)
			return;
	}
}

boolean show_menu(const menu_option* opts, char* username, MYSQL* conn) {
	return FALSE;
}
