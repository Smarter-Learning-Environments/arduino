/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <Arduino.h>
#include "module.h"
#include "debugSensor.h"

Module* module = nullptr;
Sensor* mySensor;

void setup()
{
    printf("\nHello world!\n");
    mySensor = new DebugSensor();
    pinMode(2, OUTPUT);

    module = Module::getInstance(false)->registerSensor(mySensor)->connect()->blink(4, 8);
}

void loop() {
    module->broadcast(); // TODO broker reconnect attempt if disconnected
}