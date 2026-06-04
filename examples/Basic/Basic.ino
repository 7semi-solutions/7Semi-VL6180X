/**
  * Distance And Ambient Light Example
  *
  * Description:
  * * Demonstrates simultaneous ranging and
  * ambient light measurements
  * * Prints distance in millimeters
  * * Prints ambient light in lux
  *
  * Hardware Connections:
  *
  * VL6180X    MCU
  * ---
  * VIN     -> 3.3V / 5V
  * GND     -> GND
  * SDA     -> SDA
  * SCL     -> SCL
  *
  * Notes:
  * * Distance is reported in millimeters
  * * Ambient light is reported in lux
  * * Uses single-shot measurements
  */

#include <7Semi_VL6180.h>

VL6180_7Semi sensor;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\n7Semi VL6180X Distance And ALS Example");

  if (!sensor.begin()) {
    Serial.println("VL6180X not detected");

    while (1)
      ;
  }

  Serial.println("VL6180X Ready");
  Serial.println();
}

void loop() {
  uint8_t distance;
  float lux;

  if (sensor.getDistance(distance)) {
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.print(" mm");
  } else {
    Serial.println("\nDistance measurement failed");
  }

  if (sensor.getAmbientLight(lux)) {
    Serial.print(" | Ambient  : ");
    Serial.print(lux, 2);
    Serial.print(" lux");
  } else {
    Serial.println("\nAmbient light measurement failed");
  }

  Serial.println();

  delay(500);
}
