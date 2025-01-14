#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "banque.h"

// Base de données : tableau statique de clients
static Client clients[MAX_CLIENTS];
static int nb_clients = 10; // Nombre actuel de clients dans la base

// Fonction pour trouver un client par ID
Client* trouver_client(int id_client) {
    for (int i = 0; i < nb_clients; i++) {
        if (clients[i].id_client == id_client) {
            return &clients[i];
        }
    }
    return NULL; // Client introuvable
}

// Ajouter un client à la base de données
int ajouter_client(int id_client, const char* password, double solde_initial) {
    if (nb_clients >= MAX_CLIENTS) {
        printf("Erreur : Limite maximale de clients atteinte.\n");
        return 0;
    }
    if (trouver_client(id_client) != NULL) {
        printf("Erreur : Un client avec cet ID existe déjà.\n");
        return 0;
    }

    // Ajouter le nouveau client
    clients[nb_clients].id_client = id_client;
    strncpy(clients[nb_clients].password, password, MAX_PASSWORD_LENGTH - 1);
    clients[nb_clients].solde = solde_initial;
    clients[nb_clients].nb_operations = 0;

    nb_clients++;
    printf("Client ajouté avec succès (ID : %d).\n", id_client);
    return 1;
}

// Vérifier les identifiants d'un client
int verifier_identifiants(int id_client, const char* password) {
    Client* client = trouver_client(id_client);
    if (client == NULL) {
        printf("Erreur : Client introuvable.\n");
        return 0;
    }

    if (strcmp(client->password, password) == 0) {
        return 1; // Identifiants corrects
    } else {
        printf("Erreur : Mot de passe incorrect.\n");
        return 0;
    }
}

// Dépôt d'argent
int depot(int id_client, double montant) {
    Client* client = trouver_client(id_client);
    if (client == NULL) {
        printf("Erreur : Client introuvable.\n");
        return 0;
    }

    client->solde += montant;
    snprintf(client->operations[client->nb_operations], OPERATION_LENGTH,
             "Depot : +%.2f", montant);
    client->nb_operations++;
    printf("Depot effectue avec succes.\n");
    return 1;
}

// Retrait d'argent
int retrait(int id_client, double montant) {
    Client* client = trouver_client(id_client);
    if (client == NULL) {
        printf("Erreur : Client introuvable.\n");
        return 0;
    }

    if (client->solde < montant) {
        printf("Erreur : Fonds insuffisants.\n");
        return 0;
    }

    client->solde -= montant;
    snprintf(client->operations[client->nb_operations], OPERATION_LENGTH,
             "Retrait : -%.2f", montant);
    client->nb_operations++;
    printf("Retrait effectue avec succes.\n");
    return 1;
}

// Obtenir le solde d'un client
double obtenir_solde(int id_client) {
    Client* client = trouver_client(id_client);
    if (client == NULL) {
        printf("Erreur : Client introuvable.\n");
        return -1.0; // Indique une erreur
    }
    return client->solde;
}

char** obtenir_operations(int id_client, int* nb_ops) {
    Client* client = trouver_client(id_client);
    if (client == NULL) {
        printf("Erreur : Client introuvable.\n");
        *nb_ops = -1; // Indiquer une erreur
        return NULL;
    }

    *nb_ops = client->nb_operations;
    
    // Allocation dynamique d'un tableau de pointeurs vers char
    char** operations = malloc(*nb_ops * sizeof(char*));
    if (operations == NULL) {
        printf("Erreur d'allocation memoire.\n");
        *nb_ops = -1; // Indiquer une erreur
        return NULL;
    }

    // Remplir le tableau d'opérations
    for (int i = 0; i < 10; i++) {
        operations[i] = client->operations[i]; // Assigner chaque opération
    }

    return operations; // Retourner le tableau de pointeurs
}

//
void initialiser_clients() {
    // Boucle pour initialiser les clients
    for (int i = 0; i < MAX_CLIENTS; i++) {
        // Initialisation des informations du client
        clients[i].id_client = i + 1;  // L'ID client commence à 1
        snprintf(clients[i].password, sizeof(clients[i].password), "password%d", i + 1);  // Mot de passe simple
        clients[i].solde = 1000.0;  // Solde initial fictif
        clients[i].nb_operations = 0;  // Initialisation du nombre d'opérations à 0

        // Affichage pour vérifier l'initialisation
        printf("Client %d initialise, mot de passe : %s, solde : %.2f\n", clients[i].id_client, clients[i].password, clients[i].solde);

        // Initialisation de l'historique des opérations à vide
        for (int j = 0; j < MAX_OPERATIONS; j++) {
            clients[i].operations[j][0] = '\0';  // Vide l'opération
        }
    }
}
