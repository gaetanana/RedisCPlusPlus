//
// Created by g.gonfiantini on 11/05/2023.
//
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "CREATE.h"
#include "..\ConnectionRedis\connection.h"
using namespace std;

/**
 * Cette fonction me permet de créer une clé et une valeur dans la base de données Redis
 * Demandé à l'utilisateur de saisir une clé et une valeur
 */
void createOneKeyValue(){
    redisContext *c = connectionRedis();
    string key;
    string value;
    cout << "Saisir une clé : ";
    cin >> key;
    cout << "Saisir une valeur : ";
    cin >> value;
    auto* reply = (redisReply*)redisCommand(c, "SET %s %s", key.c_str(), value.c_str());
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande SET: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    cout << "Reponse a SET: " << reply->str << "\n";
    fermertureRedis(c);
}

/**
* Cette fonction me permet de créer toutes les clés et valeurs d'un dossier dans la base de données Redis
 * En demandant le chemin absolu du dossier et tous les fichiers XML sont convertis en JSON et stockés dans la base de données Redis
 * Et le nom des clés est le nom du fichier XML
*/

void createAllKeyValue(){
    redisContext *c = connectionRedis();
    string path;
    cout << "Saisir le chemin absolu du dossier : ";
    cin >> path;
    fermertureRedis(c);
}
