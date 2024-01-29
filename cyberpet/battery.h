// This work by Digital Concepts is licensed under a Creative Commons Attribution-Noncommercial-Share Alike 2.5 Australia License. Permissions beyond the scope of this license may be available at info@digitalconcepts.net.au.  
// https://digitalconcepts.net.au/arduino/index.php?op=ESP32#wl32v3
  
#ifndef BATTERY_H
#define BATTERY_H

#include "Arduino.h"
#include "lora32_pins.h"

// Battery stuff
int volt = 0;
int volt2 = 0;
int progress = 0;
float XS = 0.0025;
// ADC resolution
const int resolution = 12;
const int adcMax = pow(2, resolution) - 1;
const float adcMaxVoltage = 3.3;
// On-board voltage divider
const int R1 = 390;
const int R2 = 100;
// Calibration measurements
const float measuredVoltage = 4.2;
const float reportedVoltage = 4.095;
// Calibration factor
const float batFactor = (adcMaxVoltage / adcMax) * ((R1 + R2)/(float)R2) * (measuredVoltage / reportedVoltage);

int setup_battery() {
  pinMode(ADC_Ctrl, OUTPUT); // pin 37
  pinMode(VBAT_Read, INPUT); // pin 1
  adcAttachPin(VBAT_Read); // default ADC_11db
  analogReadResolution(12);

  return 1;
}

float read_battery() {
    digitalWrite(ADC_Ctrl, LOW);
    delay(10);

    int analogValue = analogRead(VBAT_Read);
    digitalWrite(ADC_Ctrl, HIGH);

    return analogValue;
}

#endif
