#include "module.h"
#include <Arduino.h>

Module* Module::instance = nullptr;

Module::Module() : wifiClient(), mqttClient(wifiClient) {
    this->connect();
}

Module* Module::getInstance() {
    if(Module::instance == nullptr) {
        Module::instance = new Module();
    }
    return Module::instance;
}

void Module::connect() {
    // attempt to connect to WiFi network:
    Serial.print("Attempting to connect to WPA SSID: ");
    String ssid = "1";
    String pass = "1";
    Serial.println(ssid);

    while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
        // failed, retry
        Serial.print(".");
        delay(5000);
    }

    Serial.println("You're connected to the network");
    Serial.println();

    // You can provide a unique client ID, if not set the library uses Arduino-millis()
    // Each client must have a unique client ID
    this->clientID = millis();
    this->mqttClient.setId("Arduino_" + this->clientID);

    // You can provide a username and password for authentication
    // mqttClient.setUsernamePassword("username", "password");

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    if (!mqttClient.connect(this->broker.c_str(), this->port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        while (1);
    }

    Serial.println("You're connected to the MQTT broker!");

    this->mqttClient.onMessage(Module::setID);
    this->mqttClient.subscribe("module_assignment_service/" + this->clientID);

    // TODO handshake for module ID assignment
    this->mqttClient.beginMessage("module_assignment_service/" + this->clientID);
    this->mqttClient.print("Ready for module ID assignment");
    this->mqttClient.endMessage();
}

void Module::setID(int messageSize) {
    Module module = *Module::instance;

    Serial.println("Received a message with topic '");
    Serial.print(module.mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    String message = "";
    while (module.mqttClient.available()) {
      message += (char)module.mqttClient.read();
    }
  
    if(message == "Ready for module ID assignment") return;

    module.mqttClient.unsubscribe("module_assignment_service/" + module.clientID);

    Serial.println("Setting ID to " + message);

    module.topic += message + "/";

    module.moduleID = message.toInt();
}

void Module::registerSensor(Sensor* sensor) {
    this->sensors[this->numSensors] = *sensor;
    this->numSensors++;
}

bool Module::sendMessage(Sensor& sensor, float measurement, int time) {
    // TODO test positive, negative, NAN
    this->mqttClient.beginMessage(this->topic + sensor.topic);
    this->mqttClient.print(time);
    this->mqttClient.print(":");
    this->mqttClient.print(time);
    this->mqttClient.endMessage();
}

bool Module::broadcast() {
    // TODO sleep
    this->mqttClient.poll();

    if (this->moduleID == -1) return false;

    for(int i = 0; i < this->numSensors; i++) {
        // TODO current time each
        Sensor& sensor = this->sensors[i];
        int time = 2;
        this->sendMessage(sensor, sensor.takeMeasurement(), time);
    }
}