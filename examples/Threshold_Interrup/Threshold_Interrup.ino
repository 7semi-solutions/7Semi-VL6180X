/**
  * ThresholdInterrupt Example
  *
  * Description:
  * * Demonstrates range threshold interrupt operation
  * * GPIO1 is configured as interrupt output
  * * Interrupt is generated when the measured distance
  * falls outside the configured threshold window
  *
  * Threshold Configuration:
  * * Low Threshold  : 20 mm
  * * High Threshold : 100 mm
  *
  * Interrupt Condition:
  * * Distance < 20 mm
  * * Distance > 100 mm
  *
  * Hardware Connections:
  *
  * VL6180X    MCU
  * ---
  * VIN     -> 3.3V / 5V
  * GND     -> GND
  * SDA     -> SDA
  * SCL     -> SCL
  * GPIO1   -> GPIO (Optional Interrupt Pin)
  *
  * Notes:
  * * GPIO1 is configured as active HIGH interrupt output
  * * Interrupt status is printed when triggered
  * * Distance measurement is continuously monitored
  */

#include <7Semi_VL6180.h>

//  VL6180X sensor instance
VL6180_7Semi sensor;

void setup() {
  // Initialize serial port
  Serial.begin(115200);


  Serial.println();
  Serial.println("7Semi VL6180X Threshold Interrupt Example");

  // Initialize sensor
  if (!sensor.begin()) {
    Serial.println("VL6180X not detected");

    while (1)
      ;
  }

  /**
  * Configure threshold window
  *
  * - Trigger interrupt when:
  *   Distance < 20 mm
  *   Distance > 100 mm
  */
  if (!sensor.setThreshold(20, 100)) {
    Serial.println("Threshold configuration failed");

    while (1)
      ;
  }

  /**
  * Configure interrupt mode
  *
  * ALS:
  * - Disabled
  *
  * Range:
  * - Generate interrupt when
  *   measurement is outside
  *   threshold window
  */
  if (!sensor.setInterruptConfigGPIO(
        VL6180_Interrupt_Mode::INT_DISABLED,
        VL6180_Interrupt_Mode::INT_OUT_OF_WINDOW)) {
    Serial.println("Interrupt configuration failed");

    while (1)
      ;
  }

  /**
  * Configure GPIO1 interrupt output
  *
  * - Interrupt enabled
  * - Active HIGH polarity
  */
  if (!sensor.setGPIO1(true, true)) {
    Serial.println("GPIO1 configuration failed");

    while (1)
      ;
  }

  Serial.println("Threshold Interrupt Ready");
  Serial.println();
}

void loop() {
  uint8_t distance;
  uint8_t rangeStatus;

  // Read current distance
  if (sensor.getDistance(distance, rangeStatus)) {
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" mm");
  } else {
    Serial.print("Range Error : ");
    Serial.println(rangeStatus);
  }

  // Read interrupt status register
  uint8_t interruptStatus;

  if (sensor.getInterruptStatus(interruptStatus)) {
    /**
     * Range interrupt status
     *
     * Bits [2:0]
     */
    uint8_t rangeInterrupt = interruptStatus & 0x07;

    // Interrupt occurred
    if (rangeInterrupt != 0) {
      Serial.println();
      Serial.println("******** INTERRUPT OCCURRED ********");

      Serial.print("Interrupt Code : ");
      Serial.println(rangeInterrupt);

      Serial.print("Distance       : ");
      Serial.print(distance);
      Serial.println(" mm");

      switch (rangeInterrupt) {
        case 1:
          Serial.println("Event : Below Low Threshold");
          break;

        case 2:
          Serial.println("Event : Above High Threshold");
          break;

        case 3:
          Serial.println("Event : Outside Window");
          break;

        case 4:
          Serial.println("Event : New Sample Ready");
          break;

        default:
          Serial.println("Event : Unknown");
          break;
      }

      Serial.println("************************************");
      Serial.println();

      // Clear range interrupt
      sensor.clearRangeInterrupt();
    }
  }

  delay(100);
}
