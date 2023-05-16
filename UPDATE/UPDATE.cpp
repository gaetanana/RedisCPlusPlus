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
void updateOneHumanKey(const std::string& key, const std::string& newValue) {
    redisContext *c = connectionRedis();
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

        // Parcourir tous les éléments du document JSON
        for (auto& item : root) {
            if (item.isObject()) { // si l'élément est un objet JSON
                for (auto& innerItem : item) {
                    if (innerItem.isObject()) { // si l'élément interne est aussi un objet JSON
                        for (auto& attributeItem : innerItem) {
                            if (attributeItem.isObject()) { // si l'attribut est un objet JSON
                                // Vérifier si la clé est celle que nous recherchons et si la valeur est "Human"
                                if (attributeItem["value"].asString() == "Human") {
                                    // Si oui, mettre à jour la valeur
                                    attributeItem["value"] = newValue;
                                }
                            }
                        }
                    }
                }
            }
        }

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
