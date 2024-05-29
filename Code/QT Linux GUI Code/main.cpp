#include "widget.h"

#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <iostream>

#include "Socket_Comms.h"

//-----Need for Qt Wrapper for mosquitto-----
#include <QCoreApplication>
//-------------------------------------------

using std::cout;
using std::endl;

namespace Socket_Comms_Parameters{
    QTimer *timer;
    Socket_Comms RPi_IP_Socket;
    QString Address;
}

void checkAddress(Widget &w) { //function takes the widget object from main where its declared
    //cout << "here timer" << endl;
    Socket_Comms_Parameters::Address = Socket_Comms_Parameters::RPi_IP_Socket.get_RaspberryPi_IPAdress();
    if (!Socket_Comms_Parameters::Address.isEmpty()) {
        //cout << "inside if" << endl;
        Socket_Comms_Parameters::timer->stop();

        //cout << "before init" << endl;
        w.client.initialize("1", Socket_Comms_Parameters::Address, 1883); //initialise connection with ID 1, R_Pi IP, and port 1883
        w.client.connect(); // connect with above parameters


        Socket_Comms_Parameters::RPi_IP_Socket.Close_Socket();//IP adddress has been recieved so close the socket

        delete Socket_Comms_Parameters::timer;//delete the timer as dont need to keep calling this function
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    Socket_Comms_Parameters::RPi_IP_Socket.Socket_Setup(); //call the Socket_Setup function as has to be called before
                                                           //the get_RaspberryPi_IPAddress function

    Socket_Comms_Parameters::timer = new QTimer(); //create a timer with QTimer
    QObject::connect(Socket_Comms_Parameters::timer, &QTimer::timeout, [&w] { checkAddress(w); }); //connect the timer to call the checkAddress function and
                                                                                                   //pass the widget object W through use of a lambda function

    Socket_Comms_Parameters::timer->start(500); //call checkAddress every 0.5 seconds


    return a.exec();
}
