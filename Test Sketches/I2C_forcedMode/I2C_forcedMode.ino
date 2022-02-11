/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "Arduino.h"
#include "bme68xLibrary.h"

#define I2C_ADDR 0x76

Bme68x bme;

/**
 * @brief Initializes the sensor and hardware settings
 */
void setup(void)
{
  Wire.begin(); // Join I2C bus
  Serial.begin(115200);
  
  while (!Serial)
    delay(10);
    
  // Initialize I2C operation
  bme.begin(I2C_ADDR, Wire);

  if(bme.checkStatus())
  {
    if (bme.checkStatus() == BME68X_ERROR)
    {
      Serial.println("Sensor error:" + bme.statusString());
      return;
    }
    else if (bme.checkStatus() == BME68X_WARNING)
    {
      Serial.println("Sensor Warning:" + bme.statusString());
    }
  }
  
  /* Set the default configuration for temperature, pressure and humidity */
  bme.setTPH();

  /* Set the heater configuration to 300 deg C for 100ms for Forced mode */
  bme.setHeaterProf(300, 100);

  Serial.println("TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%), Gas resistance(ohm), Status");
}

void loop(void)
{
  bme68xData data;

  bme.setOpMode(BME68X_FORCED_MODE);
  delayMicroseconds(bme.getMeasDur());

  if (bme.fetchData())
  {
    bme.getData(data);
    Serial.println("Timestamp: " + String(millis()) + ", ");
    Serial.print("Temperature: " + String(data.temperature) + ", ");
    Serial.print("Pressure: " + String(data.pressure) + ", ");
    Serial.println("Humidity: " + String(data.humidity) + ", ");
    Serial.print("Gas: " + String(data.gas_resistance) + ", ");
    Serial.print("Status: ");
    Serial.println(data.status, HEX);
    Serial.println(); // New line
  }

  delay(1000);
}
