/**
 * Copyright (C) 2021 Bosch Sensortec GmbH
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 */

#include "Arduino.h"
#include "bme68xLibrary.h"

#define NEW_GAS_MEAS (BME68X_GASM_VALID_MSK | BME68X_HEAT_STAB_MSK | BME68X_NEW_DATA_MSK)
#define MEAS_DUR 140

#define I2C_ADDR 0x76 // Default I2C address for BME688

Bme68x bme;

/**
 * @brief Initializes the sensor and hardware settings
 */
void setup(void)
{
  Wire.begin();  // Join the I2C bus
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
      //return;
      // Wait here if there's an error
      while(1);
    }
    else if (bme.checkStatus() == BME68X_WARNING)
    {
      Serial.println("Sensor Warning:" + bme.statusString());
      // Wait here if there's a warning
      while(1);
    }
  }
  
  /* Set the default configuratioSn for temperature, pressure and humidity */
  bme.setTPH();

  /* Heater temperature in degree Celsius */
  uint16_t tempProf[10] = { 320, 100, 100, 100, 200, 200, 200, 320, 320,
      320 };
  /* Multiplier to the shared heater duration */
  uint16_t mulProf[10] = { 5, 2, 10, 30, 5, 5, 5, 5, 5, 5 };
  /* Shared heating duration in milliseconds */
  uint16_t sharedHeatrDur = MEAS_DUR - (bme.getMeasDur(BME68X_PARALLEL_MODE) / 1000);

  bme.setHeaterProf(tempProf, mulProf, sharedHeatrDur, 10);
  bme.setOpMode(BME68X_PARALLEL_MODE);

  // For debugging
  Serial.println(bme.getOpMode());
  
  Serial.println("TimeStamp(ms), Temperature(deg C), Pressure(Pa), Humidity(%), Gas resistance(ohm), Status, Gas index");
}

void loop(void)
{
  bme68xData data;
  uint8_t nFieldsLeft = 0;

  /* data being fetched for every 140ms */
  delay(MEAS_DUR);

  if (bme.fetchData())
  {
    do
    {
      nFieldsLeft = bme.getData(data);
      if (data.status == NEW_GAS_MEAS)
      {
        Serial.println("Timestamp: " + String(millis()) + ", ");
        Serial.print("Temperature: " + String(data.temperature) + ", ");
        Serial.print("Pressure: " + String(data.pressure) + ", ");
        Serial.println("Humidity: " + String(data.humidity) + ", ");
        Serial.print("Gas: " + String(data.gas_resistance) + ", ");
        Serial.print("Status: ");
        Serial.print(data.status, HEX);
        Serial.println(", Index: " + String(data.gas_index));
        Serial.println(); // New line
      }
    } while (nFieldsLeft);
  }
}
