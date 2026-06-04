# 7Semi VL6180X Arduino Library

Arduino driver for the STMicroelectronics VL6180X Time-of-Flight (ToF) distance and ambient light sensor.

The VL6180X combines a laser-based ranging sensor and ambient light sensor (ALS) in a compact package, providing accurate distance measurements and ambient light readings for proximity sensing, gesture detection, robotics, and automation applications.

---

## Features

* Distance measurement (mm)
* Ambient light measurement (lux)
* Single-shot and continuous measurement modes
* Configurable ALS gain
* Configurable ALS integration time
* Configurable measurement interval
* Interrupt support
* GPIO0 / GPIO1 configuration
* Range threshold detection
* History buffer support
* I²C address configuration
* Max convergence time configuration
* Sensor identification information

---

## Connections

### I2C

| VL6180X Pin | MCU Pin   | Notes              |
| ----------- | --------- | ------------------ |
| VIN         | 3.3V / 5V | Module dependent   |
| GND         | GND       | Common ground      |
| SDA         | SDA       | I²C data           |
| SCL         | SCL       | I²C clock          |
| GPIO0       | GPIO      | Optional           |
| GPIO1       | GPIO      | Optional interrupt |
| XSHUT       | GPIO      | Optional reset     |

**I2C Address:**

* Default I²C address: `0x29`
* Supported bus speeds:

  * 100 kHz
  * 400 kHz (recommended)

---

## Installation

### Arduino Library Manager

1. Open Arduino IDE
2. Go to Library Manager
3. Search for 7Semi VL6180X
4. Click Install

### Manual Installation

1. Download this repository as ZIP
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library

---

## Library Overview

### Distance Measurement

```cpp
uint8_t distance;

sensor.getDistance(distance);
```

* Returns distance in millimeters

### Ambient Light Measurement

```cpp
float lux;

sensor.getAmbientLight(lux);
```

* Returns ambient light level in lux

### Continuous Distance Measurement

```cpp
sensor.startContinuous();

uint8_t distance;

if (sensor.readContinuousDistance(distance))
{
    Serial.println(distance);
}
```

Stop continuous mode:

```cpp
sensor.stopContinuous();
```

### Continuous Ambient Light Measurement

```cpp
sensor.startContinuousALS();

float lux;

if (sensor.readContinuousALS(lux))
{
    Serial.println(lux);
}
```

Stop continuous mode:

```cpp
sensor.stopContinuousALS();
```

### ALS Gain Configuration

```cpp
sensor.setAlsGain(
    VL6180_ALS_Gain::VL6180_ALS_GAIN_1);
```

Available gain settings:

```cpp
VL6180_ALS_GAIN_40
VL6180_ALS_GAIN_20
VL6180_ALS_GAIN_10
VL6180_ALS_GAIN_5
VL6180_ALS_GAIN_2_5
VL6180_ALS_GAIN_1_67
VL6180_ALS_GAIN_1_25
VL6180_ALS_GAIN_1
```

### ALS Integration Time

```cpp
sensor.setIntegrationTimeALS(100);
```

Read current value:

```cpp
uint16_t integrationTime;

sensor.getIntegrationTimeALS(
    integrationTime);
```

### Measurement Interval

```cpp
sensor.setMeasurementInterval(100);
```

Read current value:

```cpp
uint16_t interval;

sensor.getMeasurementInterval(
    interval);
```

* Used in continuous ranging mode

### Interrupt Configuration

```cpp
sensor.setInterruptConfigGPIO(
    VL6180_Interrupt_Mode::INT_DISABLED,
    VL6180_Interrupt_Mode::INT_NEW_SAMPLE_READY);
```

### Threshold Configuration

```cpp
sensor.setThreshold(20, 100);
```

Read thresholds:

```cpp
uint8_t lowTh;
uint8_t highTh;

sensor.getThreshold(
    lowTh,
    highTh);
```

### GPIO Configuration

```cpp
sensor.setGPIO0(
    true,
    false,
    true);

sensor.setGPIO1(
    true,
    true);
```

### I2C Address Configuration

```cpp
sensor.setI2CAddress(0x30);
```

Read current address:

```cpp
uint8_t address;

sensor.getI2CAddress(address);
```

* Address change is temporary
* Default address restored after power cycle

### Max Convergence Time

```cpp
sensor.setMaxConvergenceTime(63);
```

Read current value:

```cpp
uint8_t timeMs;

sensor.getMaxConvergenceTime(
    timeMs);
```

* Higher values improve detection of difficult targets
* Larger values increase measurement duration

### Sensor Information

Read model ID:

```cpp
uint8_t id;

sensor.getModelId(id);
```

Read revision information:

```cpp
float modelRev;
float moduleRev;

sensor.getSensorInfo(
    modelRev,
    moduleRev);
```

Read manufacturing information:

```cpp
uint16_t year;
uint8_t month;
uint8_t day;
uint8_t phase;
uint32_t seconds;

sensor.getManufacturerInfo(
    year,
    month,
    day,
    phase,
    seconds);
```

### Interrupt Status

```cpp
uint8_t status;

sensor.getInterruptStatus(status);
```

* Returns current interrupt status flags

---

## Typical Applications

* Distance measurement
* Object detection
* Gesture sensing
* Robotics
* Presence detection
* Smart lighting
* Industrial automation
* Ambient light monitoring

---

## Dependencies

* Wire Library
* 7Semi BusCore

---
