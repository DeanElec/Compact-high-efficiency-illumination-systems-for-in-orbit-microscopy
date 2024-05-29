#include "Socket_Comms.h"

using std::cout;
using std::cerr;
using std::endl;

int Socket_Comms::Socket_Setup() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket." << endl;
        return -1;
    }

    serverAddr.sin_family = AF_INET; //set to family of ipv4
    serverAddr.sin_port = htons(8080); //this converts the port number to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error binding socket." << endl;
        return -1;
    }

    return 0;
}

const char* Socket_Comms::get_RaspberryPi_IPAdress() {
while(true){
	cout << "here soc" << endl;
    ssize_t BytesRead = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (BytesRead >= 0) {
        buffer[BytesRead] = '\0';
        cout << "Received IP address request new: " << buffer << endl;

        RaspberryPi_IPAdress = ""; //variable to store the rpi ip address in

		//send the pi IP address back to linux PC
        sendto(sock, RaspberryPi_IPAdress, strlen(RaspberryPi_IPAdress), 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    } else {
        cerr << "Error receiving data" << endl;
    }
	
	cout << "test: " << buffer << endl;
    return buffer;
	}
}

//call when want to close the socket
void Socket_Comms::closeSocket() {
    close(sock);
}

