#ifndef BANQUE_H
#define BANQUE_H

// Constantes
#define MAX_CLIENTS 10  // Taille maximale de la "base de données"
#define MAX_PASSWORD_LENGTH 20
#define MAX_OPERATIONS 10
#define OPERATION_LENGTH 100

// Structure pour représenter un client
typedef struct {
    int id_client;                  // Identifiant unique
    char password[MAX_PASSWORD_LENGTH]; // Mot de passe
    double solde;                   // Solde du compte
    char operations[MAX_OPERATIONS][OPERATION_LENGTH]; // Historique des opérations
    int nb_operations;              // Nombre d'opérations effectuées
} Client;

// Prototypes des fonctions
int verifier_identifiants(int id_client, const char* password);
int ajouter_client(int id_client, const char* password, double solde_initial);
int depot(int id_client, double somme);
int retrait(int id_client, double somme);
double obtenir_solde(int id_client);
char** obtenir_operations(int id_client, int* nb_ops);  
void initialiser_clients();

#endif // BANQUE_H
