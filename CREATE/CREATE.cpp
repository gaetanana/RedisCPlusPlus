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
Json::Value xmlNodeToJson(const pugi::xml_node& xmlNode){
    Json::Value jsonNode;
    for (pugi::xml_attribute attr = xmlNode.first_attribute(); attr; attr = attr.next_attribute()){
        jsonNode[attr.name()] = attr.value();
    }
    for (pugi::xml_node child = xmlNode.first_child(); child; child = child.next_sibling()){
        if (child.first_child().type() == pugi::node_pcdata){
            Json::Value childJson;
            childJson["value"] = child.child_value();
            for (pugi::xml_attribute attr = child.first_attribute(); attr; attr = attr.next_attribute()){
                childJson["attributes"][attr.name()] = attr.value();
            }
            jsonNode[child.name()].append(childJson);
        } else {
            jsonNode[child.name()].append(xmlNodeToJson(child));
        }
    }
    return jsonNode;
}


std::pair<string, long long> xmlToJson(string xml){
    //Chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());
    if (!result) {
        std::cout << "Erreur lors du chargement du fichier XML : " << result.description() << "\n";
        return std::make_pair("", 0);
    }
    pugi::xml_node root = doc.document_element();

    Json::Value rootJson = xmlNodeToJson(root);

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "\t";
    string jsonStr = Json::writeString(builder, rootJson);

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    //cout << "Temps d'exécution de la conversion : " << duration.count() << " microsecondes" << endl;

    return std::make_pair(jsonStr, duration.count());
}


/**
 * Cette fonction me permet de créer une clé valeur à partir d'un fichier XML
 */

void createOneKeyValueXML() {
    redisContext* c = connectionRedis();
    string path;
    string key;

    cout << "Saisir le chemin absolu du fichier : ";
    cin >> path;

    ifstream inFile;
    inFile.open(path);

    stringstream strStream;
    strStream << inFile.rdbuf(); // read the file
    string xmlStr = strStream.str(); // str holds the content of the file

    auto result = xmlToJson(xmlStr);
    string jsonStr = result.first;
    long long duration = result.second;

    // Save jsonStr in Redis with the key being the file name
    cout << "Saisir une cle : ";
    cin >> key;
    redisCommand(c,"SET %s %s", key.c_str(), jsonStr.c_str());

    fermertureRedis(c);

    cout << "Temps d'exécution de la conversion : " << duration << " microsecondes" << endl;
}

/**
* Cette fonction me permet de créer toutes les clés et valeurs d'un dossier dans la base de données Redis
 * En demandant le chemin absolu du dossier et tous les fichiers XML sont convertis en JSON et stockés dans la base de données Redis
 * Et le nom des clés est le nom du fichier XML
*/
void createAllKeyValue() {
    //Chrono pour mesurer le temps d'exécution
    auto start = chrono::high_resolution_clock::now();
    long long totalConvertTime = 0;
    int compteurNbFichier = 0;
    redisContext* c = connectionRedis();
    string path;

    cout << "Saisir le chemin absolu du dossier : ";
    cin >> path;

    for (const auto & entry : fs::directory_iterator(path)){
        if (entry.path().extension() == ".xml"){
            compteurNbFichier++;
            ifstream inFile;
            inFile.open(entry.path());

            stringstream strStream;
            strStream << inFile.rdbuf(); // read the file
            string xmlStr = strStream.str(); // str holds the content of the file

            auto result = xmlToJson(xmlStr);
            string jsonStr = result.first;
            long long duration = result.second;
            totalConvertTime += duration;

            // Save jsonStr in Redis with the key being the file name
            string key = entry.path().stem().string();
            redisCommand(c,"SET %s %s", key.c_str(), jsonStr.c_str());
        }
    }

    redisFree(c);
    //Fin du chrono
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Temps total d'exécution de l'insertion : " << duration.count() << " microsecondes" << endl;
    cout << "Temps total d'exécution des conversions : " << totalConvertTime << " microsecondes" << endl;
    cout << "Nombre de fichiers insérés : " << compteurNbFichier << endl;
}
