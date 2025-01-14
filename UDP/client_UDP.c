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

// Bibliothèques à inclure sur Windows et Linux
#include <stdio.h>          // Pour printf, puts
#include <stdlib.h>         // Pour exit, EXIT_FAILURE
#include <string.h>         // Pour memset, strcpy
#include <errno.h>

// Constantes
#define PORT 8080                  // Port du serveur
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

// Initialisation du client UDP
SOCKET init_client(SOCKADDR_IN *server_addr) {
    SOCKET client_socket;

    // 1. Création du socket UDP
    client_socket = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM pour UDP
    if (client_socket == -1) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    server_addr->sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse IP localhost (serveur sur la même machine)
    server_addr->sin_family = AF_INET;                    // IPv4
    server_addr->sin_port = htons(PORT);                  // Port 8080

    printf("Client UDP initialisé.\n");
    return client_socket;
}

// Envoi de requête au serveur UDP
void send_request(SOCKET client_socket, SOCKADDR_IN *server_addr) {
    char request[BUFFER_SIZE];
    
    // Définition de la commande à envoyer
    printf("Entrez la commande (ex: AJOUT <id_client> <id_compte> <password> <somme>) : ");
    fgets(request, BUFFER_SIZE, stdin); // Lecture de la commande

    // Envoi de la requête au serveur
    int size_sent = sendto(client_socket, request, strlen(request), 0, 
                           (SOCKADDR *)server_addr, sizeof(*server_addr));
    if (size_sent == -1) {
        perror("Erreur lors de l'envoi de la requête");
        exit(EXIT_FAILURE);
    }

    printf("Requête envoyée avec succès : (%d octets)\n", size_sent);
}

// Gestion de la réponse du serveur UDP
void handle_responses(SOCKET client_socket) {
    char response[BUFFER_SIZE]; // Tampon pour stocker la réponse
    SOCKADDR_IN server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Réception de la réponse
    int size_received = recvfrom(client_socket, response, BUFFER_SIZE - 1, 0, 
                                 (SOCKADDR *)&server_addr, &addr_len);
    if (size_received == -1) {
        perror("Erreur lors de la réception de la réponse");
        exit(EXIT_FAILURE);
    }

    response[size_received] = '\0'; // Ajouter le caractère de fin de chaîne
    printf("Réponse du serveur : %s\n", response);
}

int main() {
    SOCKET client_socket;
    SOCKADDR_IN server_addr;

    init();

    // Initialisation du client UDP
    printf("Initialisation du client UDP ... \n");
    client_socket = init_client(&server_addr);

    // Envoi d'une commande
    send_request(client_socket, &server_addr);

    // Gestion des réponses du serveur
    handle_responses(client_socket);

    closesocket(client_socket);
    end();
    return 0;
}
