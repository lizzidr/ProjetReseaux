# Projet Réseaux : Gestion de Comptes Bancaires
## Description
Ce projet est une application client-serveur permettant de gérer des comptes bancaires.
Le serveur gère les comptes des clients et traite leurs requêtes, tandis que le client permet d'envoyer des commandes au serveur.

Le projet utilise les sockets TCP/UDP pour la communication entre le client et le serveur.

## Contenu des fichiers 
- banque.h : Définit les structures et les fonctions pour la gestion des comptes bancaires.
- banque.c : Implémente les fonctions de gestion des comptes (ajout, retrait, consultation, historique).
- server.c : Serveur TCP, qui écoute les requêtes des clients et gère les commandes bancaires.
- client.c : Client TCP, qui envoie des commandes au serveur et affiche les réponses.
- Makefile : Compilation et execution.

## Compilation 
Pour générer les executables :
1. Ouvrir le terminal dans le dossier du projet (Dossier TCP ou Dossier UDP)
2. Executer la commande : make

Pour nettoyer les fichiers : 
1. Executer la commande : make clean

## Execution 
Pour lancer le serveur et le client automatiquement :
1. Executer la commande : make run

Pour lancer le serveur et le client manuellement :
1. Executer la commande : ./server (./server_UDP)
2. Executer la commande : ./client (./client_UDP)
