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
#include "json/json.h"
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
// Fonction auxiliaire pour mettre à jour les valeurs de manière récursive
void updateJsonValue(Json::Value& val, const std::string& newValue) {
    if (val.isObject()) { // Si c'est un objet
        for (auto& item : val.getMemberNames()) {
            updateJsonValue(val[item], newValue);
        }
    } else if (val.isArray()) { // Si c'est un tableau
        for (auto& item : val) {
            updateJsonValue(item, newValue);
        }
    } else if (val.isString() && val.asString() == "Human") { // Si c'est une chaîne et sa valeur est "Human"
        val = newValue;
    }
}

void updateOneHumanKey() {
    redisContext *c = connectionRedis();
    cout << "Saisir une clé : ";
    string key;
    cin >> key;

    // Récupérer la valeur de la clé de la base de données Redis
    redisReply *reply = (redisReply *)redisCommand(c, "GET %s", key.c_str());

    if (reply->type == REDIS_REPLY_STRING) {
        // Parser le document JSON
        Json::Value root;
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();
        std::string errors;

        bool parsingSuccessful = reader->parse(reply->str, reply->str + reply->len, &root, &errors);
        delete reader;

        if (!parsingSuccessful) {
            // Gérer l'échec du parsing
            std::cout << "Failed to parse JSON: " << errors << std::endl;
            return;
        }

        // Demander à l'utilisateur la nouvelle valeur
        std::cout << "Saisir une nouvelle valeur : ";
        std::string newValue;
        std::cin >> newValue;


        // Mettre à jour les valeurs dans le document JSON
        updateJsonValue(root, newValue);

        // Convertir le document JSON mis à jour en chaîne
        Json::StreamWriterBuilder writerBuilder;
        std::string updatedJson = Json::writeString(writerBuilder, root);

        // Mettre à jour la valeur dans la base de données Redis
        redisReply *setReply = (redisReply *)redisCommand(c, "SET %s %s", key.c_str(), updatedJson.c_str());

        // Libérer la mémoire allouée pour les réponses
        freeReplyObject(reply);
        freeReplyObject(setReply);
    } else {
        // Gérer le cas où la clé n'existe pas ou la valeur n'est pas une chaîne
        std::cout << "Key does not exist or value is not a string" << std::endl;
    }
}

 /**
  * Cette fonction me permet de mettre à jour toutes les valeurs de la base de données Redis qui contiennent Human
  * L'utilisateur doit donner la valeur à remplacer
  */

    void updateAllHumanKey(){

    }
