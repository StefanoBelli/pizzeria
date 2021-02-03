#ifndef MENU_ENTRIES_H
#define MENU_ENTRIES_H

#include "op.h"
#include "myutils.h"

#define ENTRIES_LEN_MANAGER 27
#define ENTRIES_LEN_BARMAN_E_PIZZAIOLO 0
#define ENTRIES_LEN_CAMERIERE 7

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
        "Incrementa disponibilita ingrediente",
        manager_inc_disp_ingrediente
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
    },
    {
        "Visualizza turno attuale",
        manager_visualizza_turno_attuale
    },
    {
        "Visualizza turni assegnati",
        manager_visualizza_turni_assegnati
    },
    {
        "Visualizza menu",
        manager_visualizza_menu
    },
    {
        "Visualizza disponibilita ingredienti",
        manager_visualizza_situazione_ingredienti
    },
    {
        "Visualizza composizione dei prodotti nel menu",
        manager_visualizza_assoc_prodotti_ingredienti
    },
    {
        "Visualizza entrate mensili",
        manager_visualizza_entrate_mensili
    },
    {
        "Visualizza entrate giornaliere",
        manager_visualizza_entrate_giornaliere
    },
    {
        "Visualizza scontrini stampati ma non pagati",
        manager_visualizza_scontrini_non_pagati
    },
    {
        "Contrassegna scontrino come pagato",
        manager_contrassegna_scontrino_pagato
    },
    {
        "Assegna tavolo a cliente",
        manager_assegna_tavolo_a_cliente
    },
    {
        "Visualizza posti per i quali è possibile stampare lo scontrino",
        manager_visualizza_tavoli_poss_stampare_scontrino
    },
    {
        "Stampa scontrino",
        manager_stampa_scontrino_tavolo_occupato
    }
};

menu_entry entries_barman_e_pizzaiolo[ENTRIES_LEN_BARMAN_E_PIZZAIOLO] = 
{

};

menu_entry entries_cameriere[ENTRIES_LEN_CAMERIERE] = 
{
    {
        "Visualizza situazione tavoli assegnati",
        cameriere_visualizza_situazione_tavoli
    },
    {
        "Prendi ordinazione da un tavolo occupato",
        cameriere_prendi_ordinazione
    },
    {
        "Chiudi ordinazione da un tavolo occupato",
        cameriere_chiudi_ordinazione
    },
    {
        "Prendi una scelta per ordinazione aperta",
        cameriere_prendi_scelta_per_ordinazione
    },
    {
        "Aggiungi ingrediente extra alla scelta",
        cameriere_aggiungi_ing_extra_per_scelta
    },
    {
        "Visualizza scelte espletate",
        cameriere_visualizza_scelte_espletate
    },
    {
        "Effettua consegna di una scelta espletata",
        cameriere_effettua_consegna
    }
};

#endif