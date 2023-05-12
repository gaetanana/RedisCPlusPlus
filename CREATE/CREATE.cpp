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
 * Cette fonction me permet de créer une clé et une valeur dans la base de données Redis
 * Demandé à l'utilisateur de saisir une clé et une valeur
 */
void createOneKeyValue(){
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
 * Cette fonction prend en paramètre du XML et le convertit en JSON
 * @param xml
 * @return
  */
string xmlToJson(string xml){
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());
    if (!result) {
        std::cout << "Erreur lors du chargement du fichier XML : " << result.description() << "\n";
        return "";
    }
    pugi::xml_node root = doc.document_element();
    Json::Value rootJson;
    pugi::xml_node child = root.first_child();
    while (child) {
        Json::Value childJson;
        pugi::xml_attribute attr = child.first_attribute();
        while (attr) {
            childJson[attr.name()] = attr.value();
            attr = attr.next_attribute();
        }
        // ajoute le texte du noeud à l'objet JSON
        if (!child.text().empty()) {
            childJson["text"] = child.text().get();
        }
        rootJson[child.name()].append(childJson);
        child = child.next_sibling();
    }

    Json::StreamWriterBuilder builder;
    builder["indentation"] = "\t";
    string jsonStr = Json::writeString(builder, rootJson);
    return jsonStr;
}


/**
* Cette fonction me permet de créer toutes les clés et valeurs d'un dossier dans la base de données Redis
 * En demandant le chemin absolu du dossier et tous les fichiers XML sont convertis en JSON et stockés dans la base de données Redis
 * Et le nom des clés est le nom du fichier XML
*/

void createAllKeyValue() {
    redisContext* c = connectionRedis();
    string path;

    cout << "Saisir le chemin absolu du dossier : ";
    cin >> path;

    // Début du chrono
    auto start = std::chrono::high_resolution_clock::now();

    for (const auto & entry : fs::directory_iterator(path)){
        if (entry.path().extension() == ".xml"){
            ifstream inFile;
            inFile.open(entry.path());

            stringstream strStream;
            strStream << inFile.rdbuf(); // read the file
            string xmlStr = strStream.str(); // str holds the content of the file

            string jsonStr = xmlToJson(xmlStr);

            // Save jsonStr in Redis with the key being the file name
            string key = entry.path().stem().string();
            redisCommand(c,"SET %s %s", key.c_str(), jsonStr.c_str());
        }
    }

    // Fin du chrono
    auto finish = std::chrono::high_resolution_clock::now();

    // Calcul de la durée d'exécution
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    redisFree(c);
}
