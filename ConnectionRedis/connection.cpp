//
// Created by g.gonfiantini on 11/05/2023.
//

#include "connection.h"
#include <iostream>
#include <hiredis/hiredis.h>
#include <WinSock2.h>

using namespace std;

redisContext* connectionRedis(){
    const char* hostname = "localhost";
    int port = 6379;
    struct timeval timeout = { 1, 500000 }; // 1.5 secondes

    std::cout << "Tentative de connexion a Redis...\n";

    redisContext* c = redisConnectWithTimeout(hostname, port, timeout);
    if (c == nullptr || c->err) {
        std::cout << "Erreur de connexion à Redis.\n";
        if (c) {
            std::cout << "Message d'erreur : " << c->errstr << "\n";
            redisFree(c);
        } else {
            std::cout << "Impossible d'allouer le contexte de redis.\n";
        }
    }
    cout << "Connecte a Redis!\n";
    return c;
}

void fermertureRedis(redisContext* c){
    redisFree(c);
}

void envoieDePingALaBase(redisContext* c){
    cout << "Envoi d'une commande PING...\n";

    auto* reply = (redisReply*)redisCommand(c, "PING");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande PING: " << c->errstr << "\n";
        redisFree(c);
    }

    std::cout << "Reponse a PING: " << reply->str << "\n";
    freeReplyObject(reply);
}