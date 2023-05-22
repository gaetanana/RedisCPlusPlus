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
void updateOneKeyValue() {
    redisContext *c = connectionRedis(); // Connexion à la base de données Redis
    //Demander à l'utilisateur la clé et la valeur à mettre à jour
    string key;
    string value;
    cout << "Saisir une clé : ";
    cin >> key;
    cout << "Saisir une valeur : ";
    cin >> value;
    //Requête pour mettre à jour la valeur de la clé
    auto *reply = (redisReply *) redisCommand(c, "SET %s %s", key.c_str(), value.c_str());
    //Condition si la requête n'a pas fonctionné
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande SET: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    //Affichage de la réponse
    cout << "Reponse a SET: " << reply->str << "\n";
    fermertureRedis(c); // Fermeture de la connexion à la base de données Redis
}

/**
 * Cette fonction me permet de mettre à jour une valeur qui contient Human et le remplace par une autre valeur
 * L'utilisateur doit donenr le nom de la clé et la valeur à remplacer
 */
// Fonction auxiliaire pour mettre à jour les valeurs de manière récursive
void updateJsonValue(Json::Value &val, const std::string &newValue) {

    // Vérifie si "val" est un objet JSON
    if (val.isObject()) {
        // Parcourt tous les membres de l'objet
        for (auto &item: val.getMemberNames()) {
            // Appelle la fonction updateJsonValue de manière récursive pour chaque membre
            updateJsonValue(val[item], newValue);
        }
    }
        // Vérifie si "val" est un tableau JSON
    else if (val.isArray()) {
        // Parcourt tous les éléments du tableau
        for (auto &item: val) {
            // Appelle la fonction updateJsonValue de manière récursive pour chaque élément
            updateJsonValue(item, newValue);
        }
    }

        // Vérifie si "val" est une chaîne de caractères et si sa valeur est "Human"
    else if (val.isString() && val.asString() == "Human") {
        // Remplace la valeur actuelle par la nouvelle valeur
        val = newValue;
    }
}

/**
 * Cette fonction me permet de mettre à jour une valeur d'une clé qui contient Human
 */
void updateOneHumanKey() {
    // Etablir la connexion avec Redis
    redisContext *c = connectionRedis();

    // Demande à l'utilisateur d'entrer une clé
    cout << "Saisir une clé : ";
    string key;
    cin >> key;

    // Récupère la valeur de la clé depuis la base de données Redis
    redisReply *reply = (redisReply *) redisCommand(c, "GET %s", key.c_str());

    // Vérifie si la réponse est une chaîne
    if (reply->type == REDIS_REPLY_STRING) {
        // Prépare le parseur JSON
        Json::Value root;
        Json::CharReaderBuilder builder;
        Json::CharReader *reader = builder.newCharReader();
        std::string errors;

        // Tente de parser la chaîne JSON récupérée
        bool parsingSuccessful = reader->parse(reply->str, reply->str + reply->len, &root, &errors);
        // Supprime le lecteur pour libérer de la mémoire
        delete reader;

        // Vérifie si le parsing a échoué
        if (!parsingSuccessful) {
            // Affiche l'erreur et termine la fonction
            std::cout << "Failed to parse JSON: " << errors << std::endl;
            return;
        }

        // Demande à l'utilisateur d'entrer une nouvelle valeur
        std::cout << "Saisir une nouvelle valeur : ";
        std::string newValue;
        std::cin >> newValue;

        // Met à jour la valeur dans le document JSON
        updateJsonValue(root, newValue);

        // Convertit le document JSON mis à jour en chaîne
        Json::StreamWriterBuilder writerBuilder;
        std::string updatedJson = Json::writeString(writerBuilder, root);

        // Met à jour la valeur dans la base de données Redis
        redisReply *setReply = (redisReply *) redisCommand(c, "SET %s %s", key.c_str(), updatedJson.c_str());

        // Libère la mémoire allouée pour les réponses
        freeReplyObject(reply);
        freeReplyObject(setReply);
    } else {
        // Gère le cas où la clé n'existe pas ou la valeur n'est pas une chaîne
        std::cout << "Key does not exist or value is not a string" << std::endl;
    }
}

/**
 * Cette fonction me permet de mettre à jour toutes les valeurs de la base de données Redis qui contiennent Human
 * L'utilisateur doit donner la valeur à remplacer
 */
void updateAllHumanKey() {
    // Démarrage du chronomètre pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    int nbFichierUpdate = 0; // Compteur du nombre de fichiers mis à jour

    // Etablir la connexion avec Redis
    redisContext *c = connectionRedis();

    // Demander à l'utilisateur d'entrer la nouvelle valeur pour remplacer 'Human'
    string newValue;
    cout << "Saisir la nouvelle valeur pour remplacer 'Human' : ";
    cin >> newValue;

    // Récupérer toutes les clés de Redis
    redisReply *reply = static_cast<redisReply *>(redisCommand(c, "KEYS *"));

    // Si la réponse est un tableau, on parcourt chaque élément
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            // Récupération de la clé
            string key = reply->element[i]->str;

            // Récupération de la valeur correspondant à la clé
            redisReply *getReply = static_cast<redisReply *>(redisCommand(c, "GET %s", key.c_str()));

            // Si la réponse est une chaîne, on poursuit le traitement
            if (getReply->type == REDIS_REPLY_STRING) {
                string jsonStr = getReply->str;

                // Conversion de la chaîne en JSON
                Json::Value root;
                std::stringstream sstr(jsonStr);
                sstr >> root;

                bool modified = false; // Indicateur de modification du JSON

                // Parcourir la structure du JSON pour trouver "Human" et remplacer par la nouvelle valeur
                for (Json::Value &value: root["tt:VideoAnalytics"]) {
                    for (Json::Value &frame: value["tt:Frame"]) {
                        for (Json::Value &object: frame["tt:Object"]) {
                            for (Json::Value &appearance: object["tt:Appearance"]) {
                                for (Json::Value &classValue: appearance["tt:Class"]) {
                                    for (Json::Value &type: classValue["tt:Type"]) {
                                        if (type["value"].asString() == "Human") {
                                            type["value"] = newValue;
                                            modified = true;
                                            nbFichierUpdate++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // Si le JSON a été modifié, on met à jour la valeur dans Redis
                if (modified) {
                    Json::StreamWriterBuilder builder;
                    builder["indentation"] = "";
                    string newJsonStr = Json::writeString(builder, root);
                    redisCommand(c, "SET %s %s", key.c_str(), newJsonStr.c_str());
                }
            }

            // Libération de la mémoire allouée pour la réponse
            freeReplyObject(getReply);
        }
    }

    // Libération de la mémoire allouée pour la réponse et déconnexion de Redis
    freeReplyObject(reply);
    redisFree(c);

    // Fin du chronomètre et affichage du temps d'exécution
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Temps d'execution : " << elapsed.count() << " s" << endl;
    cout << "Nombre d'update dans la base de donnes : " << nbFichierUpdate << endl;
}

/**
 * Cette fonction me permet de mettre à jour le contenu "Type" de toutes les valeurs de la base de données
 * Redis avec la nouvelle valeur saisie par l'utilisateurve
 */
void updateAllKeyTypeContent() {
    // Début du chronomètre pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    int nbFichierUpdate = 0; // Compteur du nombre de fichiers mis à jour

    // Etablir la connexion avec Redis
    redisContext *c = connectionRedis();

    // Demander à l'utilisateur d'entrer la nouvelle valeur
    string newValue;
    cout << "Saisir la nouvelle valeur pour remplacer la valeur actuelle : ";
    cin >> newValue;

    // Récupérer toutes les clés de Redis
    redisReply *reply = static_cast<redisReply *>(redisCommand(c, "KEYS *"));

    // Si la réponse est un tableau, on parcourt chaque élément
    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            // Récupération de la clé
            string key = reply->element[i]->str;

            // Récupération de la valeur correspondant à la clé
            redisReply *getReply = static_cast<redisReply *>(redisCommand(c, "GET %s", key.c_str()));

            // Si la réponse est une chaîne, on poursuit le traitement
            if (getReply->type == REDIS_REPLY_STRING) {
                string jsonStr = getReply->str;

                // Conversion de la chaîne en JSON
                Json::Value root;
                std::stringstream sstr(jsonStr);
                sstr >> root;

                bool modified = false; // Indicateur de modification du JSON

                // Parcourir la structure du JSON pour trouver "tt:Type" et remplacer par la nouvelle valeur
                for (Json::Value &value: root["tt:VideoAnalytics"]) {
                    for (Json::Value &frame: value["tt:Frame"]) {
                        for (Json::Value &object: frame["tt:Object"]) {
                            for (Json::Value &appearance: object["tt:Appearance"]) {
                                for (Json::Value &classValue: appearance["tt:Class"]) {
                                    for (Json::Value &type: classValue["tt:Type"]) {
                                        // Remplacement de la valeur actuelle par la nouvelle valeur
                                        type["value"] = newValue;
                                        modified = true;
                                        nbFichierUpdate++;
                                    }
                                }
                            }
                        }
                    }
                }

                // Si le JSON a été modifié, on met à jour la valeur dans Redis
                if (modified) {
                    Json::StreamWriterBuilder builder;
                    builder["indentation"] = "";
                    string newJsonStr = Json::writeString(builder, root);
                    redisCommand(c, "SET %s %s", key.c_str(), newJsonStr.c_str());
                }
            }

            // Libération de la mémoire allouée pour la réponse
            freeReplyObject(getReply);
        }
    }

    // Libération de la mémoire allouée pour la réponse et déconnexion de Redis
    freeReplyObject(reply);
    redisFree(c);

    // Fin du chronomètre et affichage du temps d'exécution
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Temps d'execution : " << elapsed.count() << " s" << endl;
    cout << "Nombre d'update dans la base de données : " << nbFichierUpdate << endl;
}


