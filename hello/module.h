#ifndef MODULE
#define MODULE

#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "sensor.h"

class Module{
    public:
        static Module* getInstance();
        void registerSensor(Sensor* sensor);
        bool broadcast();

    private:
        Module();
        static Module* instance;
        int numSensors = 0;
        int moduleID = -1;
        int clientID = -1;
        Sensor* sensors;
        WiFiClient wifiClient;
        MqttClient mqttClient;
        String broker = "test.mosquitto.org";
        int        port     = 1883;
        String topic  = "sensor_service/";
        void connect();
        static void setID(int messageSize);
        bool sendMessage(Sensor& sensor, float measurement, int time);
};

#endif