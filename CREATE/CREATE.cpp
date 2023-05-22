//
// Created by g.gonfiantini on 11/05/2023.
//
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "CREATE.h"
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
namespace fs = std::filesystem;
/**
 * Cette fonction me permet de créer une clé et une valeur dans la base de données Redis.
 * Demande à l'utilisateur de saisir une clé et une valeur.
 */
void createOneKeyValue(){
    redisContext *c = connectionRedis(); //Connexion à Redis
    //Saisie de la clé et de la valeur
    string key;
    string value;
    cout << "Saisir une clé : ";
    cin >> key;
    cout << "Saisir une valeur : ";
    cin >> value;
    //Création de la clé et de la valeur
    auto* reply = (redisReply*)redisCommand(c, "SET %s %s", key.c_str(), value.c_str());
    //Condition si la requête n'a pas fonctionné
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande SET: " << c->errstr << "\n";
        fermertureRedis(c);
    }
    //Affichage de la réponse
    cout << "Reponse a SET: " << reply->str << "\n";
    fermertureRedis(c);
}

/**
 * Cette fonction prend en paramètre du XML et le convertit en JSON
 * @param xml
 * @return
  */
// Cette fonction prend un nœud XML en entrée et le convertit en un objet JSON.
Json::Value xmlNodeToJson(const pugi::xml_node& xmlNode){
    // Initialise un nouvel objet JSON.
    Json::Value jsonNode;

    // Parcoure tous les attributs du nœud XML en entrée.
    for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute()){
        // Ajoute chaque attribut à l'objet JSON.
        jsonNode[attr.name()] = attr.value();
    }

    // Parcoure tous les nœuds enfants du nœud XML en entrée.
    for (pugi::xml_node child = xmlNode.first_child(); child; child = child.next_sibling()){
        // Si le premier enfant est un nœud texte (node_pcdata),
        // cela signifie que ce nœud a des données texte que nous devons enregistrer.
        if (child.first_child().type() == pugi::node_pcdata){
            // Crée un nouvel objet JSON pour le nœud enfant.
            Json::Value childJson;
            // Ajoute les données texte à l'objet JSON.
            childJson["value"] = child.child_value();

            // Parcoure tous les attributs du nœud enfant.
            for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute()){
                // Ajoute chaque attribut à la section "attributes" de l'objet JSON enfant.
                childJson["attributes"][attr.name()] = attr.value();
            }

            // Ajoute l'objet JSON enfant à l'objet JSON parent sous le nom du nœud enfant.
            jsonNode[child.name()].append(childJson);
        } else {
            // Si le nœud enfant n'est pas un nœud texte, nous répétons le processus
            // de conversion pour ce nœud enfant (appel récursif de la fonction).
            jsonNode[child.name()].append(xmlNodeToJson(child));
        }
    }

    // Renvoie l'objet JSON final.
    return jsonNode;
}

// Cette fonction prend une chaîne XML, la convertit en JSON, et mesure également le temps qu'il faut pour effectuer la conversion.
std::pair<string, long long> xmlToJson(string xml){
    // Enregistre l'heure actuelle.
    auto start = chrono::high_resolution_clock::now();

    // Initialise un document XML.
    pugi::xml_document doc;
    // Charge la chaîne XML dans le document.
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    // Si le chargement échoue, affiche un message d'erreur et renvoie une paire vide.
    if (!result) {
        std::cout << "Erreur lors du chargement du fichier XML : " << result.description() << "\n";
        return std::make_pair("", 0);
    }

    // Obtient le nœud racine du document XML.
    pugi::xml_node root = doc.document_element();

    // Convertit le nœud racine en JSON.
    Json::Value rootJson = xmlNodeToJson(root);

    // Initialise un générateur de chaînes JSON avec indentation.
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "\t";
    // Convertit l'objet JSON en une chaîne.
    string jsonStr = Json::writeString(builder, rootJson);

    // Enregistre l'heure actuelle.
    auto stop = chrono::high_resolution_clock::now();
    // Calcule la durée entre le début et la fin de la conversion.
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    // Renvoie la chaîne JSON et le temps qu'il a fallu pour effectuer la conversion.
    return std::make_pair(jsonStr, duration.count());
}


/**
 * Cette fonction me permet de créer une clé valeur à partir d'un fichier XML
 */
void createOneKeyValueXML() {
    redisContext* c = connectionRedis(); //Connexion à Redis
    //Demande à l'utilisateur de saisir le chemin absolu du fichier XML  et demande le nom de la clé
    string path;
    string key;

    cout << "Saisir le chemin absolu du fichier : ";
    cin >> path;

    ifstream inFile; //Création d'un flux d'entrée
    inFile.open(path); //Ouverture du fichier

    stringstream strStream;
    strStream << inFile.rdbuf(); //Lire le fichier
    string xmlStr = strStream.str(); //Str contient le contenu du fichier

    auto result = xmlToJson(xmlStr); //Convertir le XML en JSON
    string jsonStr = result.first; //Récupérer le JSON
    long long duration = result.second; //Récupérer le temps d'exécution de la conversion

    // Save jsonStr in Redis with the key being the file name
    cout << "Saisir une cle : ";
    cin >> key;
    redisCommand(c,"SET %s %s", key.c_str(), jsonStr.c_str()); //Enregistrement de la clé et de la valeur dans Redis

    fermertureRedis(c); //Fermeture de la connexion à Redis

    cout << "Temps d'exécution de la conversion : " << duration << " microsecondes" << endl;
}

/**
* Cette fonction me permet de créer toutes les clés et valeurs d'un dossier dans la base de données Redis
 * En demandant le chemin absolu du dossier et tous les fichiers XML sont convertis en JSON et stockés dans la base de données Redis
 * Et le nom des clés est le nom du fichier XML
*/
void createAllKeyValue() {
    //Chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now(); //Enregistre l'heure actuelle
    long long totalConvertTime = 0; //Comparer le temps d'exécution de la conversion de tous les fichiers
    int compteurNbFichier = 0; //Compter le nombre de fichiers insérés dans la base de données Redis
    redisContext* c = connectionRedis(); //Connexion à Redis
    string path; //Chemin absolu du dossier
    //Demande à l'utilisateur de saisir le chemin absolu du dossier
    cout << "Saisir le chemin absolu du dossier : ";
    cin >> path;

    //Parcourir tous les fichiers du dossier
    for (const auto & entry : fs::directory_iterator(path)){
        if (entry.path().extension() == ".xml"){ //Si le fichier est un fichier XML
            compteurNbFichier++; //Incrémenter le compteur de fichiers
            ifstream inFile; //Création d'un flux d'entrée
            inFile.open(entry.path()); //Ouverture du fichier

            stringstream strStream; //Création d'un flux de chaînes
            strStream << inFile.rdbuf(); //Lire le fichier
            string xmlStr = strStream.str(); //Str contient le contenu du fichier

            auto result = xmlToJson(xmlStr); //Convertir le XML en JSON
            string jsonStr = result.first; //Récupérer le JSON
            long long duration = result.second; //Récupérer le temps d'exécution de la conversion
            totalConvertTime += duration; //Ajouter le temps d'exécution de la conversion à la variable totalConvertTime

            //Sauvegarder jsonStr dans Redis avec la clé étant le nom du fichier
            string key = entry.path().stem().string(); //Récupérer le nom du fichier sans l'extension
            redisCommand(c,"SET %s %s", key.c_str(), jsonStr.c_str()); //Enregistrement de la clé et de la valeur dans Redis
        }
    }

    redisFree(c); //Fermeture de la connexion à Redis
    //Fin du chrono
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    //Affichage des résultats
    cout << "Temps total d'exécution de l'insertion : " << duration.count() << " microsecondes" << endl;
    cout << "Temps total d'exécution des conversions : " << totalConvertTime << " microsecondes" << endl;
    cout << "Nombre de fichiers insérés : " << compteurNbFichier << endl;
}
