//
// Created by g.gonfiantini on 12/05/2023.
//
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "DELETE.h"
#include "..\ConnectionRedis\connection.h"
#include "json/value.h"
#include "json/writer.h"
#include <chrono>
#include <string>
#include <filesystem>
#include <fstream>
#include <pugixml.hpp>
#include <hiredis/hiredis.h>
using namespace std;
/**
 * Cette fonction supprime une clé et une valeur de la base de données Redis
 */
void deleteOneKeyValue() {
    redisContext *c = connectionRedis(); //Connexion à Redis
    //Saisie de la clé à supprimer
    string key;
    cout << "Saisir une cle : ";
    cin >> key;
    //Suppression de la clé
    auto *reply = (redisReply *) redisCommand(c, "DEL %s", key.c_str());
    //Condition si la clé n'existe pas
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande DEL: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    //Affichage de la réponse
    cout << "Reponse a DEL: " << reply->integer << "\n";
    //Libération de la mémoire
    freeReplyObject(reply);
    //Fermeture de la connexion
    fermertureRedis(c);
}

/**
 * Cette fonction supprime toutes les clés et valeurs de la base de données Redis
 */
void deleteAllKeyValue(){
    //Chronos
    auto start = chrono::high_resolution_clock::now(); //Début du chronomètre
    redisContext *c = connectionRedis(); //Connexion à Redis

    //Récupération du nombre de clés
    auto* keyCountReply = (redisReply*)redisCommand(c, "DBSIZE");
    //Condition si la clé n'existe pas
    if (keyCountReply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande DBSIZE: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    //Nombre de clés à supprimer
    int keyCount = keyCountReply->integer;
    freeReplyObject(keyCountReply);
    //Suppression de toutes les clés
    auto* reply = (redisReply*)redisCommand(c, "FLUSHALL");
    //Condition si la clé n'existe pas on affiche un message d'erreur et on ferme la connexion
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande FLUSHALL: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    //Affichage de la réponse
    cout << "Reponse a FLUSHALL: " << reply->str << "\n";
    fermertureRedis(c);
    //On arrête le chronomètre et on affiche le temps d'exécution et le nombre de clés supprimées
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Temps d'execution de la fonction deleteAllKeyValue : " << duration.count() << " microsecondes" << endl;
    cout << "Nombre de cles supprimees : " << keyCount << endl;
}

