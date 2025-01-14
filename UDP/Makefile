# Compilateur
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS =
SRC = server_UDP.c banque.c client_UDP.c
OBJ = $(SRC:.c=.o)
TARGET_SERVER = server_UDP
TARGET_CLIENT = client_UDP

# Compilation des deux cibles
all: $(TARGET_SERVER) $(TARGET_CLIENT)

$(TARGET_SERVER): server_UDP.o banque.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TARGET_CLIENT): client_UDP.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Lancer le serveur et le client
run:
	$(MAKE) $(TARGET_SERVER) $(TARGET_CLIENT)
	gnome-terminal -- bash -c "./$(TARGET_SERVER); exec bash" & # Lancer le serveur dans un terminal
	sleep 1 # Attendre que le serveur démarre
	gnome-terminal -- bash -c "./$(TARGET_CLIENT); exec bash" & # Lancer le client dans un autre terminal

# Nettoyage des fichiers objets et binaires
clean:
	rm -f $(OBJ) $(TARGET_SERVER) $(TARGET_CLIENT)
