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
            if (choixCreate == 1) {
                createOneKeyValueXML();
            } else if (choixCreate == 2) {
                createAllKeyValue();
            } else if (choixCreate == 3) {
                std::cout << "\nVous avez quitté le menu Create\n";
            }
        } else if (choix == 2) {
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Read                                ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Lire la valeur d'une cle                                      ||\n";
            std::cout << "|| 2 - Lire toutes les cles presentes dans la base de donnees        ||\n";
            std::cout << "|| 3 - Lire toutes les cles qui ont un Human                         ||\n";
            std::cout << "|| 4 - Lire toutes les cles qui ont un Human avec                    ||\n";
            std::cout << "||     une probabilite de superieure a 0.5                           ||\n";
            std::cout << "|| 5 - Lire toutes les cles qui ont un Human avec une                ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23                                           ||\n";
            std::cout << "|| 6 - Lire toutes les cle qui ont un Human avec une                 ||\n"
                         "||     probabilite de 0.5 et les donnees datent d'apres le           ||\n"
                         "||     2023-04-21T14:45:23 et qui sont de genre masculin             ||\n";
            std::cout << "|| 7 - Quitter le menu Read                                          ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";

            int choixRead;
            std::cout << "\nVotre choix : ";
            std::cin >> choixRead;
            if (choixRead == 1) {
                readOneKeyValue();
            } else if (choixRead == 2) {
                readAllKey();
            } else if (choixRead == 3) {
                readAllKeyWithHuman();
            } else if (choixRead == 4) {
                readAllKeyWithHumanProbability();
            } else if (choixRead == 5) {
                readAllKeyWithHumanProbabilityAndDate();
            } else if (choixRead == 6) {
                readAllKeyWithHumanProbabilityAndDateGender();
            } else if (choixRead == 7) {
                std::cout << "\nVous avez quitté le menu Read\n";
            }

        } else if (choix == 3) {
            std::cout << "\n=======================================================================\n";
            std::cout << "||                          Menu Update                              ||\n";
            std::cout << "=======================================================================\n";
            std::cout << "=================== Veuillez choisir une option : ====================\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "|| 1 - Modifier une cle                                              ||\n";
            std::cout << "|| 2 - Modifier toutes les valeurs des cle (remplace Human           ||\n"
                         "|| par une autre valeur)                                             ||\n";
            std::cout << "|| 3 - Quitter le menu Update                                        ||\n";
            std::cout << "||                                                                   ||\n";
            std::cout << "=======================================================================\n";

        } else if (choix == 4) {
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
            if (choixDelete == 1) {
                deleteOneKeyValue();
            } else if (choixDelete == 2) {
                deleteAllKeyValue();
            } else if (choixDelete == 3) {
                std::cout << "\nVous avez quitté le menu Delete\n";
            }
        } else if (choix == 5) {
            cout << "========================================================\n";
            cout << "|| Vous avez quitte le programme, a bientot !         ||\n";
            cout << "========================================================\n";
            break;
        }

    }
    return 0;
}
