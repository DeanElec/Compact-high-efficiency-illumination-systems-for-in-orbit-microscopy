#include <mosquitto.h>
#include <stdio.h>
#include <cstring>

class MQTT_Publisher {
private:
    struct mosquitto *mosq;
    const char* id;
    const char* host;
    int port;

public:
//------------------------Constructor definitions---------------------------
    MQTT_Publisher(const char* _id,
    			   const char* _host,
    			   int _port)
        : id(_id),
          host(_host), 
          port(_port) {
          mosquitto_lib_init();
          mosq = mosquitto_new(id, true, NULL);
    }
//-------------------------------------------------------------------------

//------------------------------copy constructor----------------------
    MQTT_Publisher(const MQTT_Publisher& mqtt)
        : id(mqtt.id),
          host(mqtt.host),
          port(mqtt.port) {
          mosquitto_lib_init();
          mosq = mosquitto_new(id, true, NULL);
    }
//----------------------------------------------------------------------

//-----------------------equals function-------------------------------------
    MQTT_Publisher& operator=(const MQTT_Publisher& mqtt) {
        if (this == &mqtt)
            return (*this);

        id = mqtt.id;
        host = mqtt.host;
        port = mqtt.port;

        mosquitto_lib_init();
        mosq = mosquitto_new(id, true, NULL);

        return (*this);
    }
//---------------------------------------------------------------------------    
    
//------------------------------------Destructor-----------------------------
    ~MQTT_Publisher() {
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
    }
//---------------------------------------------------------------------------

//------------------------------MQTT Connect Function------------------------------
    int connect() {
        int rc = mosquitto_connect(mosq, host, port, 60);
        if(rc != 0){
            printf("Client could not connect to broker! Error Code: %d\n", rc);
            return -1;
        }
        printf("We are now connected to the broker!\n");
        return 0;
    }
//--------------------------------------------------------------------------------


//------------------------------MQTT Publish Function-----------------------------
    int publish(const char* topic, const char* message) {
        return mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
    }

    const char* get_id() { return id; }
    const char* get_host() { return host; }
    int get_port() { return port; }
};
//--------------------------------------------------------------------------------


int main() {
    MQTT_Publisher publisher("publisher-test", "localhost", 1883);
    if(publisher.connect() == 0) {
        publisher.publish("Alarm_Reset", "Temperature Alarm Reset");
    }
    return 0;
}

