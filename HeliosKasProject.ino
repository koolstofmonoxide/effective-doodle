/*

  Helios - Temperature Management System
  Script responsible for monitoring and regulating the temperature within the plant growing environment.
  Released by team Op Geluk for 'kas project' by Hogeschool Utrecht.
  30/05/2024

*/

#include <ArduinoJson.h>
#include "Arduino.h"
#include "CustomTemperatureSensor.h"
#include "CustomLinearActuator.h"
#include "CustomFanController.h"
#include "CustomSimpleElectronics.h"

// PINS
const int tempPin = 12;

const int enA = 9;
const int in1 = 8;
const int in2 = 7;

const int heaterPin = 4;

// INITIALIZERS
TemperatureSensor tempSensor(tempPin);
LinearActuator actuator(enA, in1, in2);
FanController fans(3, 2);
SimpleOnOffPart heater(heaterPin);

// GLOBAL VARIABLES
bool isWarm = false;
unsigned long warmStartTime = 0;
bool isCold = false;
unsigned long coldStartTime = 0;
int tempDiff = 0;

JsonDocument insideTempObj;
JsonDocument outsideTempObj;
JsonDocument doorObj;
JsonDocument fanObj;
JsonDocument heaterObj;
JsonDocument stateObj;

void sendJson(JsonDocument doc)
{
  serializeJson(doc, Serial);
  Serial.println();
}

void setup()
{
  Serial.begin(9600);
  tempSensor.begin();
  actuator.begin();
  fans.begin();
  heater.begin();

  heater.off();

  insideTempObj["sensor"] = "kas/Helios/insideTemp";
  outsideTempObj["sensor"] = "kas/Helios/outsideTemp";
  doorObj["sensor"] = "kas/Helios/door";
  fanObj["sensor"] = "kas/Helios/fan";
  heaterObj["sensor"] = "kas/Helios/heater";
  stateObj["sensor"] = "kas/Helios/state";

  doorObj["value"] = 0;
  fanObj["value"] = 0;
  heaterObj["value"] = 0;
  stateObj["value"] = 0; // 0 = normal, 1 = cool, 2 = warm

  //serializeJson(doc, Serial);
  //doc["sensor"] = "gps";
  //doc["time"] = 1351824120;
  //doc["value"] = 0;
}

// 16 - 20 graden
void loop()
{
  actuator.update();

  // COLLECT TEMP
  float insideTemp = tempSensor.read(0);
  float outsideTemp = tempSensor.read(1);

  // // Code for reading the temp from the serial monitor (ONLY USED FOR DEBUGGING)
  // Serial.println("Inside: ");
  // Serial.println(insideTemp);
  // Serial.println("Outside: ");
  // Serial.println(outsideTemp);
  // ADD TEMP TO JSON
  insideTempObj["value"] = insideTemp;
  outsideTempObj["value"] = outsideTemp;
  
  unsigned long currentMillis = millis(); // Variable for storing time passed

  tempDiff = outsideTemp - insideTemp;

  if (insideTemp > 20 && tempDiff > -3) // Check if it is warmer than 20 degrees
  {
    if (!isWarm)
    {
      isWarm = true;
      stateObj["value"] = 2;
      warmStartTime = currentMillis;
    }
    else if (currentMillis - warmStartTime > 60000)
    {
      coolDown();
    }
    isCold = false;
    coldStartTime = 0;
  }
  else if (insideTemp < 16 && tempDiff > 3) // Check if it is colder than 16 degrees
  {
    if (!isCold)
    {
      isCold = true;
      stateObj["value"] = 1;
      coldStartTime = currentMillis;
    }
    else if (currentMillis - coldStartTime > 60000)
    {
      warmUp();
    }
    isWarm = false;
    warmStartTime = 0;
  }
  else // Set to 'default' state (no heating or cooling)
  {
    if (isWarm || isCold)
    {
      setDefaultState();
      isWarm = false;
      warmStartTime = 0;
      isCold = false;
      coldStartTime = 0;
      stateObj["value"] = 0;
    }
  }

  sendJson(insideTempObj);
  sendJson(outsideTempObj);
  sendJson(doorObj);
  sendJson(fanObj);
  sendJson(heaterObj);
  sendJson(stateObj);
}

void setDefaultState(){
  actuator.setHeight(0);
  fans.stopFan();
  heater.off();

  doorObj["value"] = 0;
  fanObj["value"] = 0;
  heaterObj["value"] = 0;
}

void coolDown() {
  actuator.setHeight(1);
  fans.startFan();
  fans.setFanSpeed(255);
  heater.off();

  doorObj["value"] = 1;
  fanObj["value"] = 1;
  heaterObj["value"] = 0;
}

void warmUp() {
  actuator.setHeight(0);
  fans.startFan();
  fans.setFanSpeed(127); // Half the speed for noise reduction
  heater.on();

  doorObj["value"] = 0;
  fanObj["value"] = 1;
  heaterObj["value"] = 1;
}