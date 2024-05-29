#include "IPAddress.h"
#include <cstring>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

IPAddress::IPAddress() {
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void *tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        // check it is IP4 and not a loopback address
        if (ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name, "lo") != 0) { 
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, address, INET_ADDRSTRLEN);
        } 
    }
    if (ifAddrStruct != NULL) {
        freeifaddrs(ifAddrStruct);
    }
}

char* IPAddress::getAddress() {
    return address;
}

IPAddress& IPAddress::getInstance() {
    static IPAddress instance;
    return instance;
}

