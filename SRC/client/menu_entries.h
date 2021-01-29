#ifndef MENU_ENTRIES_H
#define MENU_ENTRIES_H

#include "op.h"
#include "myutils.h"

#define ENTRIES_LEN_MANAGER 14
#define ENTRIES_LEN_BARMAN_E_PIZZAIOLO 0
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
    },
    {
        "Associa prodotto e ingrediente",
        manager_associa_prodotto_e_ingrediente
    },
    {
        "Aggiungi nuovo turno",
        manager_crea_turno
    },
    {
        "Rimuovi un prodotto dal menu",
        manager_rimuovi_prodotto_del_menu
    },
    {
        "Rimuovi un ingrediente",
        manager_rimuovi_ingrediente
    },
    {
        "Rimuovi una associazione prodotto e ingrediente",
        manager_rimuovi_prodotto_e_ingrediente
    },
    {
        "Visualizza turni",
        manager_visualizza_turni
    },
    {
        "Visualizza utenti registrati",
        manager_visualizza_utenti
    },
    {
        "Visualizza situazione tavoli",
        manager_visualizza_tavoli
    },
    {
        "Assegna un turno a cameriere",
        manager_assegna_turno
    }
};

menu_entry entries_barman_e_pizzaiolo[ENTRIES_LEN_BARMAN_E_PIZZAIOLO] = 
{

};

menu_entry entries_cameriere[ENTRIES_LEN_CAMERIERE] = 
{

};

#endif