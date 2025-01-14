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

SOCKET init_client(){
    SOCKET client_socket; // Socket du serveur
    SOCKADDR_IN server_addr; // Adresse du serveur

    // 1. Socket() :
    // Création du socket du client
    client_socket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET : Pour IPv4, SOCK_STREAM : Pour TCP/IP, 0 : Pour utiliser le protocole par défaut

    if (client_socket == -1){
        perror("Erreur lors de la creation du socket");
        exit(EXIT_FAILURE);
    }

    // 2. Connect() :
    // Connexion au serveur 
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Adresse IP localhost (Serveur sur la même machine)
    server_addr.sin_family = AF_INET;                    // IPv4
    server_addr.sin_port = htons(PORT);                  // Port 8080

    // 3. Connexion au serveur
    if (connect(client_socket, (SOCKADDR *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        closesocket(client_socket);
        end();
        exit(EXIT_FAILURE);
    }

    printf("Connexion établie avec le serveur.\n");

    return client_socket;
}

// Envoi de requete
void send_request(SOCKET client_socket){
    char request[BUFFER_SIZE];
    
    // Définition de la commande à envoyer
    printf("Entrez la commande (ex: AJOUT <id_client> <id_compte> <password> <somme>) : ");
    fgets(request, BUFFER_SIZE, stdin); // Lecture de la commande du client
    
    // Envoi de la requête au serveur
    int size_sent = send(client_socket, request, strlen(request), 0); // Envoi de la commande
    if (size_sent == -1) {
        perror("Erreur lors de l'envoi de la requête");
        exit(EXIT_FAILURE);
    }

    printf("Requete envoyee avec succes : (%d octets)\n", size_sent);
}

// Reponse du serveur
void handle_responses(SOCKET client_socket) {
    char response[BUFFER_SIZE]; // Tampon pour stocker la réponse du serveur

    // Réception de la réponse du serveur
    int size_received = recv(client_socket, response, BUFFER_SIZE - 1, 0); // -1 pour laisser de la place pour le '\0'
    if (size_received == -1) {
        perror("Erreur lors de la reception de la reponse");
        exit(EXIT_FAILURE);
    }

    response[size_received] = '\0'; 
    printf("Reponse du serveur : %s\n", response);
}

int main() {
    SOCKET client_socket;
    init();

    // Initialisation du serveur
    printf("Initialisation du client ... \n");
    client_socket = init_client();

    
    // Envoi d'une commande 
    send_request(client_socket);

    // Gestion des réponses du serveur
    handle_responses(client_socket);  // Appel de la fonction pour gérer les réponses


    end();
    return 0;
}