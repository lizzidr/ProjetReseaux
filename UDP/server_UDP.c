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
#define BUFFER_SIZE 1024           // Taille du tampon pour les données reçues
#define MAX_CLIENTS 10             // Nombre maximum de clients

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

// Initialisation du serveur UDP
SOCKET init_server() {
    SOCKET server_socket;         // Socket du serveur
    SOCKADDR_IN server_addr;      // Adresse du serveur

    // Création du socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM pour UDP
    if (server_socket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;                // IPv4
    server_addr.sin_port = htons(PORT);              // Port du serveur
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Ecoute sur toutes les interfaces réseau de la machine

    // Association du socket à l'adresse et au port
    if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors du bind");
        closesocket(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP initialisé sur le port %d\n", PORT);
    return server_socket;
}

// Gestion des requêtes clients
void handle_requests(SOCKET server_socket) {
    char buffer[BUFFER_SIZE];     // Tampon pour les données reçues
    SOCKADDR_IN client_addr;      // Adresse du client
    socklen_t client_addr_len = sizeof(client_addr); // Taille de l'adresse du client

    while (1) {
        // Réception des données envoyées par le client
        int size_received = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0, (SOCKADDR*)&client_addr, &client_addr_len);
        if (size_received == -1) {
            perror("Erreur lors de la réception des données");
            continue;
        }

        buffer[size_received] = '\0'; // Terminer la chaîne
        printf("Requête reçue du client [%s:%d]: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

        // Décoder la commande et répondre
        char response[BUFFER_SIZE] = "Commande inconnue\n";
        char type_commande[10];   // Type de la commande
        int id_client, id_compte, somme;
        char password[50];

        // Décoder la commande
        int n = sscanf(buffer, "%s %d %d %s %d", type_commande, &id_client, &id_compte, password, &somme);

        // Traitement des commandes
        if (strcmp(type_commande, "AJOUT") == 0 && n == 5) {
            if (verifier_identifiants(id_client, password) && depot(id_client, somme)) {
                snprintf(response, BUFFER_SIZE, "OK\n");
            } else {
                snprintf(response, BUFFER_SIZE, "KO\n");
            }
        } else if (strcmp(type_commande, "RETRAIT") == 0 && n == 5) {
            if (verifier_identifiants(id_client, password) && retrait(id_client, somme)) {
                snprintf(response, BUFFER_SIZE, "OK\n");
            } else {
                snprintf(response, BUFFER_SIZE, "KO\n");
            }
        } else if (strcmp(type_commande, "SOLDE") == 0 && n == 4) {
            if (verifier_identifiants(id_client, password)) {
                double solde = obtenir_solde(id_client);
                snprintf(response, BUFFER_SIZE, "RES_SOLDE %.2f\n", solde);
            } else {
                snprintf(response, BUFFER_SIZE, "KO\n");
            }
        } else if (strcmp(type_commande, "OPERATIONS") == 0 && n == 4) {
            if (verifier_identifiants(id_client, password)) {
                char** operations = obtenir_operations(id_client, &n);
                response[0] = '\0';
                for (int i = 0; i < n; i++) {
                    strncat(response, operations[i], BUFFER_SIZE - strlen(response) - 1);
                    strncat(response, "\n", BUFFER_SIZE - strlen(response) - 1);
                }
            } else {
                snprintf(response, BUFFER_SIZE, "KO\n");
            }
        }

        // Envoi de la réponse au client
        int sent_size = sendto(server_socket, response, strlen(response), 0, (SOCKADDR*)&client_addr, client_addr_len);
        if (sent_size == -1) {
            perror("Erreur lors de l'envoi de la réponse");
        } else {
            printf("Réponse envoyée au client [%s:%d]: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), response);
        }
    }
}

int main() {
    SOCKET server_socket;
    init();

    // Initialiser les clients de la banque
    initialiser_clients();

    // Initialisation du serveur UDP
    server_socket = init_server();

    // Gérer les requêtes clients
    handle_requests(server_socket);

    // Fermer le socket
    closesocket(server_socket);

    end();
    return 0;
}
