#include <iostream>
#include <hiredis/hiredis.h>
#include <WinSock2.h>
#include "ConnectionRedis\connection.h"
#include "CREATE\CREATE.h"
using namespace std;

int main() {
    //Partie CREATE
    createOneKeyValue();
    //Partie DELETE

    //Partie READ

    return 0;
}
