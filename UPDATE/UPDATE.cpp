//
// Created by g.gonfiantini on 16/05/2023.
//

#include "UPDATE.h"
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
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
 * Cette fonction me permet de mettre à jour une valeur d'une clé grâce au nom de la clé
 */
void updateOneKeyValue(){
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
 * Cette fonction me permet de mettre à jour une valeur qui contient Human et le remplace par une autre valeur
 * L'utilisateur doit donenr le nom de la clé et la valeur à remplacer
 */
 void updateOneHumanKey(){

 }

 /**
  * Cette fonction me permet de mettre à jour toutes les valeurs de la base de données Redis qui contiennent Human
  * L'utilisateur doit donner la valeur à remplacer
  */

    void updateAllHumanKey(){

    }
