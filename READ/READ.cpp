//
// Created by g.gonfiantini on 12/05/2023.
//

#include "READ.h"

#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include <string>
#include <iostream>
#include "READ.h"
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


/**
 * Cette fonction lit une clé et une valeur de la base de données Redis
 */
void readAll() {
    redisContext *c = connectionRedis();
    auto* reply = (redisReply*)redisCommand(c, "KEYS *");
    if (reply == nullptr) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << c->errstr << "\n";
        fermertureRedis(c);
        return;
    }
    if (reply->type == REDIS_REPLY_ERROR) {
        std::cout << "Erreur lors de l'envoi de la commande KEYS *: " << reply->str << "\n";
    } else if (reply->type == REDIS_REPLY_ARRAY) {
        for (int i = 0; i < reply->elements; i++) {
            std::cout << "Clé " << i+1 << ": " << reply->element[i]->str << "\n";

            auto* valueReply = (redisReply*)redisCommand(c, "GET %s", reply->element[i]->str);
            if (valueReply != nullptr) {
                if (valueReply->type == REDIS_REPLY_ERROR) {
                    std::cout << "Erreur lors de l'obtention de la valeur: " << valueReply->str << "\n";
                } else if (valueReply->type == REDIS_REPLY_STRING) {
                    std::cout << "Valeur: " << valueReply->str << "\n";
                }
                freeReplyObject(valueReply);
            }
        }
    }
    freeReplyObject(reply);
    fermertureRedis(c);
}





