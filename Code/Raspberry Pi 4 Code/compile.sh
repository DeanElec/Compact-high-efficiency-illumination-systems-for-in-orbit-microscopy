#!/bin/bash

# Compile the C++ program
g++ -o MQTT_rawpy MQTT_Handling_Class_Test.cpp IPAddress.cpp MQTT_Publisher.cpp DateTime.cpp File_Monitor.cpp Socket_Comms.cpp -I /usr/include/python3.10 -L/usr/lib/python3.10/config-3.10-x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lcrypt -ldl -lm -lm -lpython3.10 -lmosquitto -lpigpio

# Check if the program compiled successfully
if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi

