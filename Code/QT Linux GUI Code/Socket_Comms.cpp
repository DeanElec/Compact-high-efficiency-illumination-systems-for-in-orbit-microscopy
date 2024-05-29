// Socket_Comms.cpp
#include "Socket_Comms.h"

#include <QTimer>
#include <QCoreApplication>

#include <fcntl.h> //for fcntl
#include <errno.h> //for errno, EAGAIN, EWOULDBLOCK

using std::cout;
using std::cerr;
using std::endl;


int Socket_Comms::Socket_Setup() {
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket." << endl;
        return -1;
    }


    fcntl(sock, F_SETFL, O_NONBLOCK); //make the socket non blocking

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
    sockaddr_in Client_Address;
    socklen_t Address_Length = sizeof(Client_Address);

    ssize_t BytesRead = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&Client_Address, &Address_Length);
    
    if (BytesRead == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //no data available
            buffer[0] = '\0';
        } else {
            cerr << "Error receiving data." << endl;
        }
    } else if (BytesRead >= 0) {
        buffer[BytesRead] = '\0'; //null-terminate received message
        cout << "Received IP address: " << buffer << endl;

        //send simple confirmation message back to the pi, could send anything pi just looks for if the length
        //of its recieve variable is greater than 0
        const char* Confirmation_Message = "IP Address Received";
        sendto(sock, Confirmation_Message, strlen(Confirmation_Message), 0, (struct sockaddr*)&Client_Address, sizeof(Client_Address));
    }

    cout << "test: " << buffer << endl;

    return buffer;
}



//call when want to close the socket
void Socket_Comms::Close_Socket() {
    close(sock);
}


