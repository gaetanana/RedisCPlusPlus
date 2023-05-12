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
    redisContext *c = connectionRedis();
    string key;
    cout << "Saisir une clé : ";
    cin >> key;
    auto *reply = (redisReply *) redisCommand(c, "DEL %s", key.c_str());
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande DEL: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    cout << "Reponse a DEL: " << reply->str << "\n";
    fermertureRedis(c);
}




/**
 * Cette fonction supprime toutes les clés et valeurs de la base de données Redis
 */

void deleteAllKeyValue(){
    redisContext *c = connectionRedis();
    auto* reply = (redisReply*)redisCommand(c, "FLUSHALL");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande FLUSHALL: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    cout << "Reponse a FLUSHALL: " << reply->str << "\n";
    fermertureRedis(c);
}

