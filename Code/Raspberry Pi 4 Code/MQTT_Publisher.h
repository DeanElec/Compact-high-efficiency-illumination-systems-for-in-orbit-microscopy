#include <mosquitto.h>
#include <stdio.h>
#include <cstring>

class MQTT_Publisher {
public:
//----------------Special Member functions-----------------
    MQTT_Publisher(const char* _id,
    			   const char* _host,
    			   int _port);
    			   
    MQTT_Publisher(const MQTT_Publisher& mqtt);
    
    MQTT_Publisher& operator=(const MQTT_Publisher& mqtt);
    
    ~MQTT_Publisher();
//---------------------------------------------------------

//-----------------get functions-----------------
    const char* get_id();
    const char* get_host();
    int get_port();
//------------------------------------------------ 

//--------------------Enabling MQTT Functions--------------------
    int connect();
    int publish(const char* topic, const char* message);  
//---------------------------------------------------------------

private:
    struct mosquitto *mosq;
    const char* id;
    const char* host;
    int port;
};


