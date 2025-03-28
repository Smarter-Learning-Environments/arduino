//hello

#include "module.h"
#include "debugSensor.h"

Module* module;

void setup() {
  module = Module::getInstance();
  DebugSensor* debugSensor = new DebugSensor();
  module->registerSensor(debugSensor);
}

void loop() {
  module->broadcast();
}