/**
* Continuous Distance Measurement Example
*
* Description:
* * Demonstrates continuous ranging mode
* * Sensor automatically performs measurements
* * Distance is read whenever a new sample is available
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
* * Measurement interval is set to 100 ms
* * Distance is reported in millimeters
* * Continuous mode reduces measurement overhead
*/

#include <7Semi_VL6180.h>

VL6180_7Semi sensor;

void setup()
{

    Serial.begin(115200);
    delay(1000);
    Serial.println("\n7Semi VL6180X Continuous Distance Example");

    if (!sensor.begin())
    {
        Serial.println("VL6180X not detected");

        while (1)
            ;
    }

    /**
     * Configure measurement interval
     *
     * - New measurement every 100 ms
     */
    sensor.setMeasurementInterval(100);

    /**
     * Start continuous ranging mode
     */
    while (!sensor.startContinuous())
    {
        Serial.println("Failed to start continuous mode");
    }

    Serial.println("Continuous Ranging Started");
    Serial.println();
}

void loop()
{
    uint8_t distance, status;

    if (sensor.readContinuousDistance(distance))
    {
        Serial.print("Distance : ");
        Serial.print(distance);
        Serial.println(" mm");
    }
}
