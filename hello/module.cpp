#include "module.h"
#include <Arduino.h>
#include <stdio.h>
#include <string>
#include <sstream>

Module* Module::instance = nullptr;

Module::Module() : Module(false) {}

Module::Module(bool deferConnection) : wifiClient(), mqttClient(wifiClient), sensors(), http() {
    if(!deferConnection) {
        this->connect();
    }
}

Module* Module::getInstance() {
    return getInstance(false);
}

Module* Module::getInstance(bool deferConnection) {
    if(Module::instance == nullptr) {
        Module::instance = new Module(deferConnection);
    }
    return Module::instance;
}

Module* Module::connect() {
    // attempt to connect to WiFi network:
    printf("Attempting to connect to WPA SSID: \n");
    printf(ssid.c_str());
    printf("\n");
    WiFi.begin(ssid.c_str(), pass.c_str());
    while (WiFi.status() != WL_CONNECTED) {
        // failed, retry
        printf(".");
        fflush(stdout);
        delay(500);
    }

    printf("\nYou're connected to the network\n");

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    printf("Attempting to connect to backend service...\n");
    http.begin(this->wifiClient, "http://" + broker + ":8000/discover-module");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("accept", "application/json");
    std::ostringstream jsonBody;
    jsonBody << "{\"hw_id\":\"" << mac.c_str() << "\",\"sensor_count\": " << this->sensors.size() << " }";
    // TODO pass sensor descriptions...
    printf(jsonBody.str().c_str());
    int httpResponseCode = http.POST(jsonBody.str().c_str());
    printf("HTTP Response code: \n");
    printf(std::to_string(httpResponseCode).c_str());
    http.end();
    if(!(httpResponseCode == 200 || httpResponseCode == 409)) {
        exit(1); // TODO retry instead of restart
    }


    printf("\nAttempting to connect to the MQTT broker: \n");
    printf(broker.c_str());
    printf("\n");

    mqttClient.setServer(this->broker.c_str(), this->port);

    if (!mqttClient.connect(mac.c_str())) {
        printf("MQTT connection failed!\n");

        delay(5000);
    }

    printf("You're connected to the MQTT broker!\n");

    return this;
}

Module* Module::registerSensor(Sensor* sensor) {
    this->sensors.push_back(sensor);
    return this;
}

bool Module::sendMessage(int sensorID, float measurement, int time) {
    // TODO test positive, negative, NAN
    this->mqttClient.publish((this->topic + String(std::to_string(sensorID).c_str())).c_str(), (std::to_string(time) + ":" + std::to_string(measurement)).c_str());
    return true;
}

bool Module::broadcast() {
    // TODO sleep
    this->mqttClient.loop();
    delay(1000);
    printf("Spinning\n");


    for(int i = 0; i < this->sensors.size(); i++) {
        Sensor* sensor = this->sensors[i];
        // TODO current time each
        // TODO millis() rollover - manage here or db
        int time = -1;
        this->sendMessage(i, sensor->takeMeasurement(), time);
    }

    return false;
}

Module* Module::blink(int duration, int freq) {
    for(int i = 0; i < duration; i++) {
        this->mqttClient.loop();
        for(int j = 0; j < freq; j++) {
            printf("LED_ON\n");
            delay(1000 / freq / 2);
            printf("LED_OFF\n");
            delay(1000 / freq / 2);
        }
    }

    return this;
}