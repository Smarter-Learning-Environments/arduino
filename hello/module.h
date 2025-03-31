#ifndef MODULE
#define MODULE

#include <Arduino.h>
#include <ArduinoMqttClient.h>
#include <WiFi101.h>
#include "sensor.h"
#include "secrets.h"

class Module{
    public:
        static Module* getInstance();
        void registerSensor(Sensor* sensor);
        bool broadcast();
        String ssid = SECRET_SSID;
        String pass = SECRET_PASS;

    private:
        Module();
        static Module* instance;
        int numSensors = 0;
        int moduleID = MODULE_UUID;
        long clientID = -1;
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