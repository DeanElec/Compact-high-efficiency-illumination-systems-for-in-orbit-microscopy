#!/bin/bash

# Compile the C++ program
g++ -o MQTT_rawpy MQTT_Handling_Class_Test.cpp IPAddress.cpp MQTT_Publisher.cpp DateTime.cpp File_Monitor.cpp Socket_Comms.cpp PWM_Control.cpp -I /usr/include/python3.11 -I/usr/include/python3.11 -Wsign-compare -g	-fstack-protector-strong -Wformat -Werror=format-security -DNDEBUG -g -fwrapv -O2 -Wall -L/usr/lib/python3.11/config-3.11-aarch64-linux-gnu -L/usr/lib/aarch64-linux-gnu -ldl -lm -lpython3.11 -lmosquitto -lpigpio -std=c++17

# Check if the program compiled successfully
if [ $? -eq 0 ]; then
    echo "Compilation successful."
else
    echo "Compilation failed."
fi

