//
// Created by g.gonfiantini on 12/05/2023.
//

#include "READ.h"

#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "READ.h"
#include "..\ConnectionRedis\connection.h"
#include "json/value.h"
#include "json/writer.h"
#include <json/json.h>
#include <chrono>
#include <string>
#include <filesystem>
#include <fstream>
#include <pugixml.hpp>
#include <hiredis/hiredis.h>
#include <sstream>
using namespace std;


/**
 * Cette fonction lit une clé et une valeur de la base de données Redis
 */
void readOneKeyValue() {
    redisContext *c = connectionRedis();
    string key;
    cout << "Saisir une cle : ";
    cin >> key;
    auto *reply = (redisReply *) redisCommand(c, "GET %s", key.c_str());
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande GET: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande GET: " << reply->str << "\n";
    } else if (reply->type == REDIS_REPLY_STRING) {
        std::cout << "Reponse a GET: " << reply->str << "\n";
    }
    freeReplyObject(reply);
    fermertureRedis(c);
}


/**
 * Cette fonction lit une clé et une valeur de la base de données Redis
 */
void readAllKey() {
    redisContext *c = connectionRedis();
    auto* reply = (redisReply*)redisCommand(c, "KEYS *");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << reply->str << "\n";
    } else if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            std::cout << "Cle " << i+1 << ": " << reply->element[i]->str << "\n";

            auto* valueReply = (redisReply*)redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                if (valueReply->type == REDIS_REPLY_ERROR) {
                    std::cout << "Erreur lors de l'obtention de la valeur: " << valueReply->str << "\n";
                } else if (valueReply->type == REDIS_REPLY_STRING) {
                    std::cout << "Valeur: " << valueReply->str << "\n";
                }
                freeReplyObject(valueReply);
            }
        }
    }
    freeReplyObject(reply);
    fermertureRedis(c);
}

/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * 1 filtre
 */

void readAllKeyWithHuman(){
    redisContext *c = connectionRedis();
    auto* reply = (redisReply*)redisCommand(c, "KEYS *");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << reply->str << "\n";
    } else if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {

            auto* valueReply = (redisReply*)redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                if (valueReply->type == REDIS_REPLY_ERROR) {
                    std::cout << "Erreur lors de l'obtention de la valeur: " << valueReply->str << "\n";
                }
                else if (valueReply->type == REDIS_REPLY_STRING) {
                    std::string valueStr(valueReply->str);
                    // Vérifier si la valeur contient "tt:Type"
                    std::size_t found = valueStr.find("\"tt:Type\"");
                    if (found != std::string::npos) {
                        // Vérifier si la valeur contient "Human"
                        std::size_t foundHuman = valueStr.find("Human", found);
                        if (foundHuman != std::string::npos) {
                            std::cout << "Cle " << i+1 << ": " << reply->element[i]->str << "\n";
                            //std::cout << "Valeur: " << valueReply->str << "\n";
                        }
                    }
                }
                freeReplyObject(valueReply);
            }
        }
    }
    freeReplyObject(reply);
    fermertureRedis(c);
}

/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * Elle permet de filtrer les valeurs qui possèdent une probabilité supérieur à 0.5
 * 2 filtres
 */
#include <json/json.h>

void readAllKeyWithHumanProbability() {
    redisContext *c = connectionRedis();
    auto *reply = (redisReply *)redisCommand(c, "KEYS *");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << reply->str << "\n";
    } else if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            auto *valueReply = (redisReply *)redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                if (valueReply->type == REDIS_REPLY_ERROR) {
                    std::cout << "Erreur lors de l'obtention de la valeur: " << valueReply->str << "\n";
                } else if (valueReply->type == REDIS_REPLY_STRING) {
                    std::string valueStr(valueReply->str);
                    // Parse the JSON string
                    Json::Value root;
                    Json::CharReaderBuilder builder;
                    std::string errs;
                    std::istringstream iss(valueStr);
                    if (!Json::parseFromStream(builder, iss, &root, &errs)) {
                        std::cout << "Erreur lors de l'analyse de la valeur JSON: " << errs << "\n";
                    } else {
                        const Json::Value& videoAnalytics = root["tt:VideoAnalytics"];
                        for (int j = 0; j < videoAnalytics.size(); j++) {
                            const Json::Value& frames = videoAnalytics[j]["tt:Frame"];
                            for (int k = 0; k < frames.size(); k++) {
                                const Json::Value& objects = frames[k]["tt:Object"];
                                for (int l = 0; l < objects.size(); l++) {
                                    const Json::Value& appearances = objects[l]["tt:Appearance"];
                                    for (int m = 0; m < appearances.size(); m++) {
                                        const Json::Value& classes = appearances[m]["tt:Class"];
                                        for (int n = 0; n < classes.size(); n++) {
                                            const Json::Value& types = classes[n]["tt:Type"];
                                            for (int o = 0; o < types.size(); o++) {
                                                const Json::Value& attributes = types[o]["attributes"];
                                                if (types[o]["value"].asString() == "Human" &&
                                                    attributes.isMember("Likelihood") &&
                                                    std::stod(attributes["Likelihood"].asString()) > 0.5) {
                                                    std::cout << "Cle " << i+1 << ": " << reply->element[i]->str << "\n";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                freeReplyObject(valueReply);
            }
        }
    }
    freeReplyObject(reply);
    fermertureRedis(c);
}







bool dateIsAfter(const std::string& dateTimeStr, const std::string& filterDate) {
    std::istringstream dateTimeStream(dateTimeStr);
    std::istringstream filterStream(filterDate);
    std::tm dateTimeTm = {};
    std::tm filterTm = {};

    dateTimeStream >> std::get_time(&dateTimeTm, "%Y-%m-%dT%H:%M:%SZ");
    filterStream >> std::get_time(&filterTm, "%Y-%m-%dT%H:%M:%SZ");

    auto dateTimeTp = std::chrono::system_clock::from_time_t(std::mktime(&dateTimeTm));
    auto filterTp = std::chrono::system_clock::from_time_t(std::mktime(&filterTm));

    return dateTimeTp > filterTp;
}

/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * Elle permet de filtrer les valeurs qui possèdent une probabilité supérieur à 0.5
 * Elle permet de filtrer les valeurs qui possèdent une date supérieur à la date mise dans le filtre (qui est dans le code)
 * 3 filtres
 */
void readAllKeyWithHumanProbabilityAndDate() {
    // Création d'un pointeur sur le contexte Redis
    redisContext* c = connectionRedis();

    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
        } else {
            std::cout << "Cannot allocate redis context." << std::endl;
        }
        return;
    }
    // Obtenir toutes les clés de la base de données Redis
    redisReply* reply = (redisReply*)redisCommand(c, "KEYS *");

    // Parcourir chaque clé
    for (int i = 0; i < reply->elements; i++) {
        redisReply* keyReply = (redisReply*)redisCommand(c, "GET %s", reply->element[i]->str);

        // Analyser le JSON
        Json::Reader reader;
        Json::Value root;
        bool parsingSuccessful = reader.parse(keyReply->str, root);

        if (!parsingSuccessful) {
            std::cout << "Failed to parse JSON." << std::endl;
            freeReplyObject(keyReply);
            continue;
        }

        // Filtrer les valeurs qui possèdent le type "Human"
        const Json::Value typeValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["value"];
        if (typeValue.asString() != "Human") {
            freeReplyObject(keyReply);
            continue;
        }

        // Filtrer les valeurs qui possèdent une probabilité supérieure à 0.5
        const Json::Value likelihoodValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["attributes"]["Likelihood"];
        if (stod(likelihoodValue.asString()) <= 0.5) {
            freeReplyObject(keyReply);
            continue;
        }

        // Filtrer les valeurs qui possèdent une date supérieure à la date mise dans le filtre
        const Json::Value dateValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["UtcTime"];
        std::string dateTime = dateValue.asString();
        std::tm tm = {};
        std::istringstream ss(dateTime);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        //std::cout << "Date: " << dateTime << std::endl; //A ce stade il y a pas le filtre date
        // Définir la date de filtre
        //Actuellement le filtre est sur 2023-04-21T14:45:23 c'est à dire que toutes les valeurs qui ont une date inférieur à celle-ci ne seront pas affichées
        std::string filterDateTimeStr = "2023-04-21T14:45:23";  // Modifiez cette valeur en conséquence

        std::tm filterTm = {};
        std::istringstream filterSs(filterDateTimeStr);
        filterSs >> std::get_time(&filterTm, "%Y-%m-%dT%H:%M:%S");
        auto filterTp = std::chrono::system_clock::from_time_t(std::mktime(&filterTm));

        if (tp < filterTp) {
            freeReplyObject(keyReply);
            continue;
        }


        // Imprimer les valeurs qui passent tous les filtres
        std::cout << "Cle " << i+1 << ": " << reply->element[i]->str << "\n";
        //std::cout << keyReply->str << std::endl;
        freeReplyObject(keyReply);
    }

    // Libération de la mémoire
    freeReplyObject(reply);

    // Fermeture de la connexion
    fermertureRedis(c);
}
/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * Elle permet de filtrer les valeurs qui possèdent une probabilité supérieur à 0.5
 * Elle permet de filtrer les valeurs qui possèdent une date supérieur à la date mise dans le filtre (qui est dans le code)
 * Permet de filtrer le genre Masculin
 * 4 filtres
 */


void readAllKeyWithHumanProbabilityAndDateGender(){

}
