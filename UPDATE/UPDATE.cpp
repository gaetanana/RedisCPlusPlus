//
// Created by g.gonfiantini on 15/05/2023.
//

#include "UPDATE.h"
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "UPDATE.h"
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
* Cette fonction permet de modifier une valeur d'une clé en lui le nom de la clé et une nouvelle valeure
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

