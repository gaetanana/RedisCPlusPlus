#include <iostream>
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include "ConnectionRedis\connection.h"
#include "CREATE\CREATE.h"
#include "DELETE\DELETE.h"
#include "READ\READ.h"
#include "UPDATE\UPDATE.h"
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
    //readAllKeyWithHumanProbabilityAndDate();
    //readAllKeyWithHumanProbabilityAndDateGender();

    //Partie application

    while (true) {
        std::cout << "\n========================================================\n";
        std::cout << "|| Programme de test de la base de donnees Redis      ||\n";
        std::cout << "========================================================\n";
        std::cout << "============== Veuillez choisir une option : ===========\n";
        std::cout << "||                                                    ||\n";
        std::cout << "|| 1 - Create                                         ||\n";
        std::cout << "|| 2 - Read                                           ||\n";
        std::cout << "|| 3 - Update                                         ||\n";
        std::cout << "|| 4 - Delete                                         ||\n";
        std::cout << "|| 5 - Quitter le programme                           ||\n";
        std::cout << "||                                                    ||\n";
        std::cout << "========================================================\n";
        int choix;
        std::cout << "Votre choix : ";
        std::cin >> choix;
        std::cout << "\n";
        if (choix == 1) {
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Create                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Creer une cle avec un fichier XML en valeur                   ||\n";
            std::cout << "|| 2 - Stocker l'ensemble des fichiers XML d'un dossier dans Redis   ||\n";
            std::cout << "|| 3 - Quitter le menu Create                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "========================================================================\n";
            int choixCreate;
            std::cout << "\nVotre choix : ";
            std::cin >> choixCreate;
            if(choixCreate == 1 ){
                createOneKeyValueXML();
            }else if(choixCreate == 2){
                createAllKeyValue();
            }else if(choixCreate == 3){
                std::cout << "\nVous avez quitté le menu Create\n";
            }
        }
        else if(choix == 2){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Read                                ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Lire la valeur d'une cle                                      ||\n";
            std::cout << "|| 2 - Lire toutes les cles presentes dans la base de donnees        ||\n";
            std::cout << "|| 3 - Lire toutes les cles qui a dans les valeurs un humain         ||\n";
            std::cout << "|| 4 - Lire toutes les cles qui a dans les valeurs un humain avec    ||\n";
            std::cout << "||     une probabilite de survie superieure a 0.5                    ||\n";
            std::cout << "|| 5 - Lire toutes les cle qui a dans les valeurs un humain avec une ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23                                           ||\n";
            std::cout << "|| 6 - Lire toutes les cle qui a dans les valeurs un humain avec une ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23 et qui sont de genre masculin             ||\n";
            std::cout << "|| 7 - Quitter le menu Read                                          ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";

            int choixRead;
            std::cout << "\nVotre choix : ";
            std::cin >> choixRead;
            if(choixRead == 1){
                readOneKeyValue();
            }else if(choixRead == 2){
                readAllKey();
            }
            else if(choixRead == 3){
                readAllKeyWithHuman();
            }
            else if(choixRead == 4){
                readAllKeyWithHumanProbability();
            }
            else if(choixRead == 5){
                readAllKeyWithHumanProbabilityAndDate();
            }
            else if(choixRead == 6){
                readAllKeyWithHumanProbabilityAndDateGender();
            }
            else if(choixRead == 7){
                std::cout << "\nVous avez quitté le menu Read\n";
            }

        }
        else if(choix == 3){
            cout << "========================================================\n";
            cout << "|| Update n'est pas encore disponible                  ||\n";
            cout << "========================================================\n";
        }

        else if(choix == 4){
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Delete                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Supprimer une cle                                             ||\n";
            std::cout << "|| 2 - Supprimer toutes les cles                                     ||\n";
            std::cout << "|| 3 - Quitter le menu Delete                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";
            int choixDelete;
            std::cout << "\nVotre choix : ";
            std::cin >> choixDelete;
            if(choixDelete == 1){
                deleteOneKeyValue();
            }else if(choixDelete == 2){
                deleteAllKeyValue();
            }else if(choixDelete == 3){
                std::cout << "\nVous avez quitté le menu Delete\n";
            }
        }
        else if(choix == 5){
            cout << "========================================================\n";
            cout << "|| Vous avez quitte le programme, a bientot !         ||\n";
            cout << "========================================================\n";
            break;
        }

    }
    return 0;
}
