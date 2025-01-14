// Si sur Windows
#ifdef _WIN32 /* si vous êtes sous Windows */

#include <winsock2.h> 
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Si sur Linux
#else /* si vous êtes sous Linux */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#endif

// Bibliothéques à inclure sur Windows et Linux
#include <stdio.h>          // Pour printf, puts
#include <stdlib.h>         // Pour exit, EXIT_FAILURE
#include <string.h>         // Pour memset, strcpy
#include <errno.h>
#include "banque.h"



// Constantes
#define PORT 8080                  // Port du serveur
#define MAX_CLIENTS 10             // Nombre de clients maximum
#define BUFFER_SIZE 1024           // Taille du tampon où seront stockées les données


static void init(void)
{
#ifdef _WIN32
    WSADATA wsa;
    int err = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(err < 0)
    {
        puts("WSAStartup failed !");
        exit(EXIT_FAILURE);
    }
#endif
}

static void end(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}


SOCKET init_server(){
    SOCKET server_socket; // Socket du serveur
    SOCKADDR_IN server_addr; // Adresse du serveur

    // 1. Socket() :
    // Création du socket du serveur
    server_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET : Pour IPv4, SOCK_STREAM : Pour TCP/IP, 0 : Pour utiliser le protocole par défaut

    if (server_socket == -1){
        perror("Erreur lors de la creation du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Adresse internet : Ecoute sur toutes les interfaces reseaux de la machine (y compris localhost) 
    server_addr.sin_family = AF_INET;                 // Famille : AF_INET
    server_addr.sin_port = htons(PORT);               // Numéro port : 8080

    // 2. Bind() :
    // Association du socket à l'adresse IP et au port
    if(bind (server_socket, (SOCKADDR *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Erreur lors du bind");
        closesocket(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen() : 
    // Mise en écoute du serveur des connexions entrantes
    if(listen(server_socket, MAX_CLIENTS) == -1){
        perror("Erreur lors de l'écoute");
        closesocket(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Serveur initialise et en ecoute sur le port : %d \n", PORT);
    printf("Adresse IP serveur : %s \n", inet_ntoa(server_addr.sin_addr));

    return server_socket;
}

// Connexion d'un client
SOCKET accept_client(SOCKET server_socket) {
    SOCKADDR_IN client_addr;           // Structure pour l'adresse du client
    socklen_t client_addr_len = sizeof(client_addr); // Taille de la structure

    // Accepter une connexion entrante
    SOCKET client_socket = accept(server_socket, (SOCKADDR *)&client_addr, &client_addr_len);

    if (client_socket == INVALID_SOCKET) {
        perror("Erreur lors de l'acceptation");
        exit(EXIT_FAILURE);
    }

    // Afficher l'adresse IP du client connecté
    printf("Client connecte : %s\n", inet_ntoa(client_addr.sin_addr));

    return client_socket; // Retourner le socket du client
}

// Gestion des requetes
int handle_requests(int client_socket) {
    char buffer[BUFFER_SIZE]; // Allocation de la zone mémoire où les données reçues seront stockées

    ////////////////////////////////////////
    // Paramètres de la commande reçue //

    // Variables pour stocker les données extraites
    char type_commande[10];   // Type de la commande
    int id_client, id_compte, somme;  // Id client, Id compte, Somme
    char password[50];   // Mot de passe

    // Réception des données envoyées par le client
    int size_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0); 
    if (size_received == -1) {
        perror("Erreur de reception");
        return -1;
    }

    buffer[size_received] = '\0';  // Ajouter un caractère de fin de chaîne (null-terminator)
    printf("Requete recue : %s \n", buffer);

    // Décodage de la commande reçue
    int n = sscanf(buffer, "%s %d %d %s %d", type_commande, &id_client, &id_compte, password, &somme);

    // Vérifier si la commande est valide
    if (!(n == 5 || n == 4)) {
        printf("Erreur de format dans la commande recue\n");
        return -1;  // Erreur de format
    }

    // Affichage des données extraites 
    printf("Commande : %s\n", type_commande);
    printf("ID Client : %d\n", id_client);
    printf("ID Compte : %d\n", id_compte);
    printf("Mot de passe : %s\n", password);
    if (n == 5) printf("Somme : %d\n", somme);

    ///////////////////////////////////////////////////////
    // Décoder les commandes en fonction des paramètres //
    if (strcmp(type_commande, "AJOUT") == 0 && n == 5) {
        // Commande AJOUT : Ajouter une somme sur le compte
        if (verifier_identifiants(id_client, password)) {  // Correction : id_client et password
            if (depot(id_client, somme)) {
                send(client_socket, "OK\n", 3, 0);
            } else {
                send(client_socket, "KO\n", 3, 0);
            }
        } else {
            send(client_socket, "KO\n", 3, 0);
        }
    } else if (strcmp(type_commande, "RETRAIT") == 0 && n == 5) {
        // Commande RETRAIT : Retirer une somme du compte
        if (verifier_identifiants(id_client, password)) {  // Correction : id_client et password
            if (retrait(id_client, somme)) {
                send(client_socket, "OK\n", 3, 0);
            } else {
                send(client_socket, "KO\n", 3, 0);
            }
        } else {
            send(client_socket, "KO\n", 3, 0);
        }
    } else if (strcmp(type_commande, "SOLDE") == 0 && n == 4) {
        // Commande SOLDE : Obtenir le solde d’un compte
        if (verifier_identifiants(id_client, password)) {  // Correction : id_client et password
            double solde = obtenir_solde(id_client);
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "RES_SOLDE - %.2f\n", solde); 
            send(client_socket, response, strlen(response), 0);
        } else {
            send(client_socket, "KO\n", 3, 0);
        }
    } else if (strcmp(type_commande, "OPERATIONS") == 0 && n == 4) {
        // Commande OPERATIONS : Obtenir les 10 dernières opérations
        if (verifier_identifiants(id_client, password)) {  // Correction : id_client et password
            char operations[10][BUFFER_SIZE]; // Tampon pour 10 opérations
            int nb_ops = 0;  // Initialiser nb_ops
            char** operations_ptr = obtenir_operations(id_client, &nb_ops);  // Obtenir les opérations

            // Si des opérations existent, les concaténer dans une seule chaîne
            if (operations_ptr != NULL) {
                char all_operations[BUFFER_SIZE] = "\nRES_OP - ";  // Initialiser une chaîne vide pour stocker toutes les opérations
                for (int i = 0; i < nb_ops; i++) {
                    // Ajouter chaque opération suivie d'un délimiteur '\n'
                    strncat(all_operations, operations_ptr[i], BUFFER_SIZE - strlen(all_operations) - 1);
                    strncat(all_operations, "\n", BUFFER_SIZE - strlen(all_operations) - 1); // Ajouter un saut de ligne entre les opérations
                }

                // Maintenant all_operations contient toutes les opérations concaténées
                send(client_socket, all_operations, strlen(all_operations), 0); // Envoi de la chaîne complète
            }

        } else {
            send(client_socket, "KO\n", 3, 0);
        }
    } else {
        // Commande inconnue ou mal formatée
        send(client_socket, "KO\n", 3, 0);
    }

    return 0;
}


int main() {
    SOCKET server_socket;
    SOCKET client_socket;
    init();

    // Initialiser les clients 
    initialiser_clients();

    // Initialisation du serveur
    printf("Initialisation du serveur ... \n");
    server_socket = init_server();

    
    while(1){
        // Attente des connexions des clients
        client_socket = accept_client(server_socket);

        handle_requests(client_socket);
    }

    closesocket(client_socket);
    closesocket(server_socket);

    end();
    return 0;
}