//
// Created by g.gonfiantini on 11/05/2023.
//

#include "hiredis/hiredis.h"

#ifndef REDISCPLUSPLUS_CONNECTION_H
#define REDISCPLUSPLUS_CONNECTION_H

#endif //REDISCPLUSPLUS_CONNECTION_H
redisContext* connectionRedis();
void fermertureRedis(redisContext* c);
void envoieDePingALaBase(redisContext* c);