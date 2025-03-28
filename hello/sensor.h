#ifndef SENSOR
#define SENSOR

#include <Arduino.h>

class Sensor {
    public:
        Sensor(String sensorType) {
            this->topic = sensorType;
        }
        virtual float takeMeasurement() = 0;
        String topic;
};

#endif