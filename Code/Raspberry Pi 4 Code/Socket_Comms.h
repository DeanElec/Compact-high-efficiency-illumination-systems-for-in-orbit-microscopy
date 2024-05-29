#ifndef SOCKET_COMMS_H
#define SOCKET_COMMS_H

#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

class Socket_Comms {
public:
    int Socket_Setup();
    const char* get_RaspberryPi_IPAdress();
    void closeSocket();

private:
    const char* RaspberryPi_IPAdress;
    int sock;
    char buffer[1024];
    sockaddr_in serverAddr;
};

#endif

