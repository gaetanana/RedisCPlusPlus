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
    //cout << xmlToJson("<root><child1>value1</child1><child2>value2</child2></root>");

    //Partie DELETE
    //deleteOneKeyValue(); //Fonction qui permet de supprimer une clé et une valeur dans la base de données Redis
    deleteAllKeyValue(); //Fonction qui permet de supprimer toutes les clés et valeurs dans la base de données Redis

    //Partie CREATE
    //createOneKeyValue(); //Fonction qui permet de créer une clé et une valeur dans la base de données Redis
    createAllKeyValue(); //Fonction qui permet de insérer toutes les clés (nom du fichier) et les valeurs (contenue du fichier) dans la base de données Redis
    //createOneKeyValueXML(); //Fonction qui permet de créer une clé et une valeur dans la base de données Redis à partir d'un fichier XML



    //Partie READ
    //readAllKey();

    return 0;
}
