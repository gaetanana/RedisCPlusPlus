#include <iostream>
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include "ConnectionRedis\connection.h"
#include "CREATE\CREATE.h"
#include "DELETE\DELETE.h"
#include "READ\READ.h"
using namespace std;

int main() {
    //Partie test des fonctions
    /*cout << xmlToJson("<tt:MetadataStream xmlns:tt=\"http://www.onvif.org/ver10/schema\" xmlns:fc=\"http://www.onvif.org/ver20/analytics/humanface\" xmlns:bd=\"http://www.onvif.org/ver20/analytics/humanbody\">\n"
                      "<tt:VideoAnalytics>\n"
                      "<tt:Frame UtcTime=\"2023-04-21T14:40:20.72Z\" AplSource=\"AIPeopleDetection\">\n"
                      "<tt:Object ObjectId=\"5504\">\n"
                      "<tt:Appearance>\n"
                      "<tt:Shape>\n"
                      "<tt:BoundingBox left=\"-0.94\" top=\"-0.01\" right=\"-0.84\" bottom=\"-0.58\"/>\n"
                      "<tt:CenterOfGravity x=\"-0.89\" y=\"-0.29\"/>\n"
                      "</tt:Shape>\n"
                      "<tt:Class>\n"
                      "<tt:Type Likelihood=\"0.37\">Human</tt:Type>\n"
                      "</tt:Class>\n"
                      "</tt:Appearance>\n"
                      "<tt:Extension xmlns=\"\">\n"
                      "<Properties>\n"
                      "<Property name=\"DirectionNamed\">Left</Property>\n"
                      "<Property name=\"Color\">1</Property>\n"
                      "</Properties>\n"
                      "</tt:Extension>\n"
                      "</tt:Object>\n"
                      "</tt:Frame>\n"
                      "</tt:VideoAnalytics>\n"
                      "</tt:MetadataStream>");*/

    //Test de la fonction dateIsAfter qui permet de comparer deux dates

    //cout << dateIsAfter("2023-03-21T14:40:20.72Z", "2023-04-21T14:40:20.72Z") << endl; //Return 0 qui est false
    //cout << dateIsAfter("2023-05-21T14:40:20.72Z", "2023-04-21T14:40:20.72Z") << endl; //Return 1 qui est true

    //--------------------------------------------------------------------------------------------------------------

    //Partie DELETE
    //deleteOneKeyValue(); //Fonction qui permet de supprimer une clé et une valeur dans la base de données Redis
    //deleteAllKeyValue(); //Fonction qui permet de supprimer toutes les clés et valeurs dans la base de données Redis

    //Partie CREATE
    //createOneKeyValue(); //Fonction qui permet de créer une clé et une valeur dans la base de données Redis
    //createAllKeyValue(); //Fonction qui permet de insérer toutes les clés (nom du fichier) et les valeurs (contenue du fichier) dans la base de données Redis
    //createOneKeyValueXML(); //Fonction qui permet de créer une clé et une valeur dans la base de données Redis à partir d'un fichier XML



    //Partie READ
    //readAllKey();
    //readAllKeyWithHuman();
    //readAllKeyWithHumanProbability();
    readAllKeyWithHumanProbabilityAndDate();

    return 0;
}
