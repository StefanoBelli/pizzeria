#ifndef MENU_ENTRIES_H
#define MENU_ENTRIES_H

#include "op.h"
#include "myutils.h"

#define ENTRIES_LEN_MANAGER 5
#define ENTRIES_LEN_BARMAN 0
#define ENTRIES_LEN_PIZZAIOLO 0
#define ENTRIES_LEN_CAMERIERE 0

menu_entry entries_manager[ENTRIES_LEN_MANAGER] = 
{
    {
        "Crea nuovo utente", 
        manager_crea_nuovo_utente
    },
    {
        "Ripristina password utente esistente",
        manager_ripristina_password_utente_esistente
    },
    {
        "Aggiungi un nuovo tavolo",
        manager_aggiungi_nuovo_tavolo
    },
    {
        "Aggiungi un nuovo ingrediente",
        manager_aggiungi_nuovo_ingrediente
    },
    {
        "Aggiungi prodotto del menu",
        manager_aggiungi_prodotto_del_menu
    }
};

menu_entry entries_barman[ENTRIES_LEN_BARMAN] = 
{

};

menu_entry entries_pizzaiolo[ENTRIES_LEN_PIZZAIOLO] = 
{

};

menu_entry entries_cameriere[ENTRIES_LEN_CAMERIERE] = 
{

};

#endif