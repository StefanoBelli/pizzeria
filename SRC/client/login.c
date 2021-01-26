#include "global_config.h"
#include "mybool.h"
#include "op.h"

typedef enum {
	ROLE_UNKNOWN, //login fallito
	ROLE_MANAGER,
	ROLE_CAMERIERE,
	ROLE_PIZZAIOLO,
	ROLE_BARMAN
} role;

static void set_menu_based_on_role(role r) {
	if(r == ROLE_MANAGER) {
		cfg.menu_entries = entries_manager;
		cfg.menu_entries_len = ENTRIES_LEN_MANAGER;
    } else if(r == ROLE_CAMERIERE) {
		cfg.menu_entries = entries_cameriere;
		cfg.menu_entries_len = ENTRIES_LEN_CAMERIERE;
    } else if (r == ROLE_BARMAN) {
		cfg.menu_entries = entries_barman;
		cfg.menu_entries_len = ENTRIES_LEN_BARMAN;
    } else if (r == ROLE_PIZZAIOLO) {
		cfg.menu_entries = entries_pizzaiolo;
		cfg.menu_entries_len = ENTRIES_LEN_PIZZAIOLO;
    }
}

mybool attempt_login(const char* username, const char* password) {
	return FALSE; //if role r = unknown
}