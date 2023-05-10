#include <iostream>
#include <hiredis/hiredis.h>
#include <WinSock2.h>

int main() {
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
        return 1;
    }

    std::cout << "Connecte a Redis! Envoi d'une commande PING...\n";

    redisReply* reply = (redisReply*)redisCommand(c, "PING");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande PING: " << c->errstr << "\n";
        redisFree(c);
        return 1;
    }

    std::cout << "Reponse a PING: " << reply->str << "\n";
    freeReplyObject(reply);

    std::cout << "Recuperation de toutes les cles...\n";

    reply = (redisReply*)redisCommand(c, "KEYS *");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        redisFree(c);
        return 1;
    }

    if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            std::cout << "Cle : " << reply->element[i]->str << "\n";

            redisReply* valueReply = (redisReply*)redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                std::cout << "Valeur : " << valueReply->str << "\n";
                freeReplyObject(valueReply);
            }
        }
    }

    freeReplyObject(reply);
    redisFree(c);

    return 0;
}
