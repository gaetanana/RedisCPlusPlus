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
    redisContext *c = connectionRedis(); //Connexion à Redis
    //Demande de la clé à lire
    string key;
    cout << "Saisir une cle : ";
    cin >> key;
    auto *reply = (redisReply *) redisCommand(c, "GET %s", key.c_str());
    //Condition si la clé est nulle
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande GET: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }

    //Si la requête renvoie une erreur
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande GET: " << reply->str << "\n";
    }
    //Affichage de la réponse de la requête
    else if (reply->type == REDIS_REPLY_STRING) {
        std::cout << "Reponse a GET: " << reply->str << "\n";
    }
    freeReplyObject(reply); //Libération de la mémoire
    fermertureRedis(c); //Fermeture de la connexion
}


/**
 * Cette fonction lit toutes les clés et valeurs de la base de données Redis
 */
void readAllKey() {
    auto start = chrono::high_resolution_clock::now(); //Début du chronomètre
    int nbCle = 0; //Compteur de clés

    redisContext *c = connectionRedis(); //Connexion à Redis
    auto *reply = (redisReply *) redisCommand(c, "KEYS *"); //Récupération de toutes les clés
    //Condition si la clé est nulle
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    //Si la requête renvoie une erreur
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << reply->str << "\n";
    }
    //Si la requête renvoie un tableau
    else if (reply->type == REDIS_REPLY_ARRAY) {
        //Parcours du tableau
        for (int i = 0; i < reply->elements; i++) {
            std::cout << "Cle " << i + 1 << ": " << reply->element[i]->str << "\n";
            nbCle++;

            //Récupération de la valeur de la clé
            auto *valueReply = (redisReply *) redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                if (valueReply->type == REDIS_REPLY_ERROR) {
                    std::cout << "Erreur lors de l'obtention de la valeur: " << valueReply->str << "\n";
                } else if (valueReply->type == REDIS_REPLY_STRING) {
                    //std::cout << "Valeur: " << valueReply->str << "\n";
                }
                freeReplyObject(valueReply); //Libération de la mémoire
            }
        }
    }
    freeReplyObject(reply); //Libération de la mémoire
    fermertureRedis(c); //Fermeture de la connexion
    //Fin du chronomètre et affichage du temps d'exécution et du nombre de clés lues
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Nombre de cle : " << nbCle << endl;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
}

/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * 1 filtre
 */
void readAllKeyWithHuman() {
    //Chrono
    auto start = chrono::high_resolution_clock::now();
    int nbCle = 0;
    // Création d'un pointeur sur le contexte Redis
    redisContext *c = connectionRedis();

    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
        } else {
            std::cout << "Cannot allocate redis context." << std::endl;
        }
        return;
    }
    // Obtenir toutes les clés de la base de données Redis
    auto *reply = (redisReply *) redisCommand(c, "KEYS *");

    // Parcourir chaque clé
    for (int i = 0; i < reply->elements; i++) {

        auto *keyReply = (redisReply *) redisCommand(c, "GET %s", reply->element[i]->str);

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
        if (typeValue.asString() == "Human") { // Ici, on compare si la valeur est "Human"
            std::cout << "Cle " << i + 1 << ": " << reply->element[i]->str << "\n";
            nbCle++;
        }
        freeReplyObject(keyReply);
    }

    // Libération de la mémoire
    freeReplyObject(reply);

    // Fermeture de la connexion
    fermertureRedis(c);
    //Fin chrono
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Nombre de cle : " << nbCle << endl;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
}

/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * Elle permet de filtrer les valeurs qui possèdent une probabilité supérieur à 0.5
 * 2 filtres
 */
void readAllKeyWithHumanProbability() {
    //Chrono
    auto start = chrono::high_resolution_clock::now();
    int nbCle = 0;
    // Création d'un pointeur sur le contexte Redis
    redisContext *c = connectionRedis();
    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
        } else {
            std::cout << "Cannot allocate redis context." << std::endl;
        }
        return;
    }
    // Obtenir toutes les clés de la base de données Redis
    auto *reply = (redisReply *) redisCommand(c, "KEYS *");
    // Parcourir chaque clé
    for (int i = 0; i < reply->elements; i++) {
        auto *keyReply = (redisReply *) redisCommand(c, "GET %s", reply->element[i]->str);
        // Analyser le JSON
        Json::Reader reader;
        Json::Value root;
        bool parsingSuccessful = reader.parse(keyReply->str, root);
        if (!parsingSuccessful) {
            std::cout << "Failed to parse JSON." << std::endl;
            freeReplyObject(keyReply);
            continue;
        }
        // Parcourir tous les tt:Appearance dans tt:Object
        for (const auto &object: root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"]) {
            // Filtrer les valeurs qui possèdent le type "Human"
            const Json::Value typeValue = object["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["value"];
            if (typeValue.asString() != "Human") {
                continue;
            }

            // Filtrer les valeurs qui possèdent une probabilité supérieure à 0.5
            const Json::Value likelihoodValue = object["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["attributes"]["Likelihood"];
            double likelihoodDouble = stod(likelihoodValue.asString());

            if (likelihoodDouble >= 0.50) {
                nbCle++;
                break;  // Arrêt de la boucle for
            }
        }
        freeReplyObject(keyReply);
    }
    // Libération de la mémoire
    freeReplyObject(reply);
    // Fermeture de la connexion
    fermertureRedis(c);
    //Fin chrono
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Nombre de cle : " << nbCle << endl;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
}


bool dateIsAfter(const std::string &dateTimeStr, const std::string &filterDate) {
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
    // Démarrer le chronomètre
    auto start = chrono::high_resolution_clock::now();
    int keyCount = 0;  // Compteur de clés qui passent les filtres

    // Connexion à Redis
    redisContext *c = connectionRedis();

    // Si la connexion échoue, afficher l'erreur et quitter la fonction
    if (c == NULL || c->err) {
        if (c) {
            cout << "Error: " << c->errstr << endl;
        } else {
            cout << "Cannot allocate redis context." << endl;
        }
        return;
    }

    // Récupérer toutes les clés de Redis
    auto *reply = (redisReply *) redisCommand(c, "KEYS *");

    // Parcourir toutes les clés
    for (int i = 0; i < reply->elements; i++) {
        // Récupérer la valeur pour la clé actuelle
        auto *keyReply = (redisReply *) redisCommand(c, "GET %s", reply->element[i]->str);

        // Analyser la valeur en tant que JSON
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(keyReply->str, root)) {
            cout << "Failed to parse JSON." << endl;
            freeReplyObject(keyReply);
            continue;
        }

        // Parcourir les données de video analytics dans le JSON
        const auto &videoAnalytics = root["tt:VideoAnalytics"];
        for (const auto &analytics: videoAnalytics) {
            const auto &frames = analytics["tt:Frame"];
            for (const auto &frame: frames) {
                const auto &objects = frame["tt:Object"];
                for (const auto &object: objects) {
                    const auto &appearances = object["tt:Appearance"];
                    for (const auto &appearance: appearances) {
                        const auto &classes = appearance["tt:Class"];
                        for (const auto &cls: classes) {
                            const auto &types = cls["tt:Type"];
                            for (const auto &type: types) {
                                // Récupérer les valeurs nécessaires pour les filtres
                                string valueType = type["value"].asString();
                                double valueLikelihood = std::stod(type["attributes"]["Likelihood"].asString());
                                string utcTime = frame["UtcTime"].asString();

                                // Appliquer les filtres : le type doit être "Human", la probabilité doit être supérieure à 0,5, et la date doit être postérieure à "2023-04-21T14:45:23"
                                if (valueType == "Human" && valueLikelihood > 0.5 && utcTime > "2023-04-21T14:45:23") {
                                    // Imprimer la clé si elle passe tous les filtres
                                    cout << "Cle " << i + 1 << ": " << reply->element[i]->str << "\n";
                                    keyCount++;
                                }
                            }
                        }
                    }
                }
            }
        }

        // Libérer la mémoire allouée pour la réponse à la requête GET
        freeReplyObject(keyReply);
    }

    // Libérer la mémoire allouée pour la réponse à la requête KEYS
    freeReplyObject(reply);
    // Fermer la connexion à Redis
    fermertureRedis(c);

    // Arrêter le chronomètre et imprimer le temps d'exécution et le nombre de clés qui passent les filtres
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
    cout << "Nombre de cle : " << keyCount << "\n";
}


/**
 * Cette fonction permet de retrouver toutes les clé-valeur de la base de données Redis
 * Elle permet de filtrer les valeurs qui possèdent le type "Human"
 * Elle permet de filtrer les valeurs qui possèdent une probabilité supérieur à 0.5
 * Elle permet de filtrer les valeurs qui possèdent une date supérieur à la date mise dans le filtre (qui est dans le code)
 * Permet de filtrer le genre Masculin
 * 4 filtres
 */
void readAllKeyWithHumanProbabilityAndDateGender() {
    // Démarrer le chronomètre
    auto start = chrono::high_resolution_clock::now();
    int nbCle = 0; // Compteur de clés qui passent les filtres

    // Connexion à Redis
    redisContext *c = connectionRedis();

    // Si la connexion échoue, afficher l'erreur et quitter la fonction
    if (c == NULL || c->err) {
        if (c) {
            std::cout << "Error: " << c->errstr << std::endl;
        } else {
            std::cout << "Cannot allocate redis context." << std::endl;
        }
        return;
    }

    // Récupérer toutes les clés de Redis
    redisReply *reply = (redisReply *) redisCommand(c, "KEYS *");

    // Parcourir toutes les clés
    for (int i = 0; i < reply->elements; i++) {
        // Récupérer la valeur pour la clé actuelle
        redisReply *keyReply = (redisReply *) redisCommand(c, "GET %s", reply->element[i]->str);

        // Analyser la valeur en tant que JSON
        Json::Reader reader;
        Json::Value root;
        bool parsingSuccessful = reader.parse(keyReply->str, root);

        // Si l'analyse échoue, afficher une erreur et passer à la clé suivante
        if (!parsingSuccessful) {
            std::cout << "Failed to parse JSON." << std::endl;
            freeReplyObject(keyReply);
            continue;
        }

        // Appliquer le premier filtre : le type doit être "Human"
        const Json::Value typeValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["value"];
        if (typeValue.asString() != "Human") {
            freeReplyObject(keyReply);
            continue;
        }

        // Appliquer le deuxième filtre : la probabilité doit être supérieure à 0.5
        const Json::Value likelihoodValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Class"][0]["tt:Type"][0]["attributes"]["Likelihood"];
        if (stod(likelihoodValue.asString()) <= 0.5) {
            freeReplyObject(keyReply);
            continue;
        }

        // Appliquer le troisième filtre : la date doit être postérieure à une certaine date
        const Json::Value dateValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["UtcTime"];
        std::string dateTime = dateValue.asString();
        std::tm tm = {};
        std::istringstream ss(dateTime);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

        std::string filterDateTimeStr = "2023-05-16T00:00:00";  // Cette date est le filtre
        std::tm filterTm = {};
        std::istringstream filterSs(filterDateTimeStr);
        filterSs >> std::get_time(&filterTm, "%Y-%m-%dT%H:%M:%S");
        auto filterTp = std::chrono::system_clock::from_time_t(std::mktime(&filterTm));

        if (tp < filterTp) {
            freeReplyObject(keyReply);
            continue;
        }

        // Appliquer le quatrième filtre : le genre doit être "Masculin" avec une probabilité supérieure à 0.5
        if (!root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Extension"][0]["HumanFace"][0]["Gender"][0]["Male"][0].isNull()) {
            const Json::Value genderValue = root["tt:VideoAnalytics"][0]["tt:Frame"][0]["tt:Object"][0]["tt:Appearance"][0]["tt:Extension"][0]["HumanFace"][0]["Gender"][0]["Male"][0]["value"];
            if (stod(genderValue.asString()) > 0.5) {
                std::cout << "Cle " << i + 1 << ": " << reply->element[i]->str << "\n";
                nbCle++;
            }
        }

        // Libérer la mémoire allouée pour la réponse à la requête GET
        freeReplyObject(keyReply);
    }

    // Libérer la mémoire allouée pour la réponse à la requête KEYS
    freeReplyObject(reply);

    // Fermer la connexion à Redis
    redisFree(c);

    // Arrêter le chronomètre et afficher le temps d'exécution et le nombre de clés qui passent les filtres
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
    cout << "Nombre de cle : " << nbCle << "\n";
}



