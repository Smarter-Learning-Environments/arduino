//hello

#include <Arduino.h>
#include "module.h"
#include "debugSensor.h"

Module* module;

void setup() {
  randomSeed(analogRead(0));

  module = Module::getInstance();
  DebugSensor* debugSensor = new DebugSensor();
  module->registerSensor(debugSensor);
}

void loop() {
  module->broadcast();
}