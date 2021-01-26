#include "global_config.h"
#include "mybool.h"

typedef enum {
	ROLE_UNKNOWN, //login fallito
	ROLE_MANAGER,
	ROLE_CAMERIERE,
	ROLE_PIZZAIOLO,
	ROLE_BARMAN
} role;

static void set_menu_based_on_role(role r) {

}

mybool attempt_login(const char* username, const char* password) {
	return FALSE;
}