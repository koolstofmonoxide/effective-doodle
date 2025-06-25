# TCS3472 color sensor / ClosestColor
Voor de TCS3472 kleurensensor heb ik een library geschreven voor het uitlezen van de sensor in RGB. Vervolgens met de ClosestColor script kan de dichtstbijzijnde kleur worden bepaald. Dit laatste als demonstratie van de werking van de sensor:

```cpp
/*
  Example for TCS3472 combined with an I2C LCD-screen.
  Created by Co Bijkerk on 03/06/2024.
*/

#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "ClosestColor.h"
#include "TCS3472.h"

TCS3472 colorSensor;
ClosestColor closestColorFinder;
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct TargetColor {
    const char* name;
};

TargetColor colorlist[] = {
    {"Red"},
    {"Lime"},
    {"Blue"},
    {"Yellow"},
    {"Cyan"},
};

const int numColors = sizeof(colorlist) / sizeof(TargetColor);
TargetColor targetColor;

bool colorSensorStarted = false;

void setup() {
    Serial.begin(9600);
    if (!colorSensor.begin()) {
        Serial.println("Failed to start sensor.");
    }
    else {
        colorSensorStarted = true;
    }

    lcd.init();
    lcd.backlight();

    randomSeed(analogRead(0));
    selectNewColor();
}

void loop() {
    uint16_t r, g, b; 
    colorSensor.getRGB(r, g, b);

    String detectedColor = closestColorFinder.getClosestColor(r, g, b);
    Serial.println(detectedColor);

    if (detectedColor == targetColor.name) {
        delay(1000);
        selectNewColor();
    }

    delay(500);
}

void selectNewColor() {
    if (colorSensorStarted = true){
        targetColor = colorlist[random(0, numColors)];

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Show me: ");
        lcd.print(targetColor.name);

        Serial.print("New target color: ");
        Serial.println(targetColor.name);
    }
    else {
        lcd.print("Failed to start");
    }
}
```

# Helios Kas project
Voor het tweede semester op de Hogeschool Utrecht moesten wij een kas automatiseren. Een onderdeel hiervan was het managen van de temperatuur. Dit met twee temperatuursensoren, een actuator, ventilatoren en een verwarmingselement.

Vervolgens werd er via een webinterface actuale data vertoond van de kas.

# speedDetection
Voor het derde project moesten wij een snelheidsmeter maken voor ouderen. Hier gebruikte we LiDAR voor om de afstand te meten. Vervolgens met deze afstand wordt de snelheid berekend.

Hier zit vervolgens ook een filter in. Destijds heb ik onderzocht welke filter het beste werkt (tussen de mediaanfilter en moving average filter).

Een groot onderdeel van dit hele project was multithreading. Hier is FreeRTOS voor gebruikt.