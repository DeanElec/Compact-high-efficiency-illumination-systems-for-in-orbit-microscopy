#include <netinet/in.h>

class IPAddress {
private:
    char address[INET_ADDRSTRLEN];
    

public:
	IPAddress();
    static IPAddress& getInstance();
    char* getAddress();
};

