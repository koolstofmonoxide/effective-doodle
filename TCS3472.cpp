#include "TCS3472.h"
#include "TCS3472_ENUM.h"

TCS3472::TCS3472() {}

void TCS3472::getRGB(uint16_t &r, uint16_t &g, uint16_t &b) // Lezen van de RGB values
{
    r = readFromSensor(COMMAND::RDATAL);
    g = readFromSensor(COMMAND::GDATAL);
    b = readFromSensor(COMMAND::BDATAL);
}

void TCS3472::writeToSensor(uint8_t reg, uint8_t value) // Schrijven naar sensor
{
    Wire.beginTransmission(_address);
    Wire.write(COMMAND::COMMAND_ | reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint16_t TCS3472::readFromSensor(uint8_t reg) // Lezen van sensor
{
    uint16_t value;
    Wire.beginTransmission(_address);
    Wire.write(COMMAND::COMMAND_ | reg);
    Wire.endTransmission();

    Wire.requestFrom(_address, (uint8_t)2);
    value = Wire.read();
    value |= (Wire.read() << 8);

    return value;
}

bool TCS3472::begin(uint8_t address) // Starten van de sensor
{
    _address = address;
    Wire.begin();

    Wire.beginTransmission(_address);
    if (Wire.endTransmission() != 0)
    {
        return false;
    }

    enable();
    writeToSensor(COMMAND::ATIME, COMMAND::SET_ATIME);

    return true;
}

void TCS3472::enable() // Aanzetten van de sensor
{
    writeToSensor(COMMAND::ENABLE, COMMAND::ENABLE_PON);
    delay(3);
    writeToSensor(COMMAND::ENABLE, COMMAND::ENABLE_AEN);
}