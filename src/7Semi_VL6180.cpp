#include "7Semi_VL6180.h"

VL6180_7Semi::VL6180_7Semi()
{
}

VL6180_7Semi::~VL6180_7Semi()
{
    if (bus)
    {
        delete bus;
        bus = nullptr;
    }
}

bool VL6180_7Semi::begin(uint8_t addr, TwoWire &i2cPort, uint32_t i2cClock)
{
    // Delete previous bus instance if already initialized
    if (bus)
    {
        delete bus;
        bus = nullptr;
    }

    // Initialize I2C interface
    if (!i2c.beginI2C(addr, i2cPort, i2cClock))
        return false;

    // Create BusCore communication wrapper
    bus = new BusIO_7Semi<I2C_7Semi, 2, 1>(i2c);

    // Verify allocation
    if (!bus)
        return false;

    // Read sensor model ID
    uint8_t id = 0;

    if (!getModelId(id))
        return false;

    // Verify VL6180X device
    if (id != VL6180_IDENTIFICATION_MODEL_ID)
        return false;

    // Read fresh-out-of-reset status
    uint8_t status = 0;

    if (!bus->read(VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET, status))
        return false;

    // Apply mandatory ST initialization sequence after power-up
    if (status & 0x01)
    {
        if (!defaultSetting())
            return false;

        // Clear fresh reset flag
        if (!bus->write(VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET, 0x00))
            return false;
    }

    // Configure GPIO interrupt for new sample ready event
    if (!bus->write(VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, (uint8_t)0x24))
        return false;

    // Configure GPIO1 active-high output
    if (!bus->write(VL6180_REG_SYSTEM_MODE_GPIO1, (uint8_t)0x10))
        return false;

    // Set ranging intermeasurement period
    if (!bus->write(VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, (uint8_t)0x09))
        return false;

    // Configure ALS gain = 1x
    if (!bus->write(VL6180_REG_SYSALS_ANALOGUE_GAIN, (uint8_t)0x46))
        return false;

    // Configure VHV repeat rate
    if (!bus->write(VL6180_REG_SYSRANGE_VHV_REPEAT_RATE, (uint8_t)0xFF))
        return false;

    // Clear fresh reset flag
    if (!bus->write(VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET, (uint8_t)0x00))
        return false;

    // Set ALS integration time to 100ms
    if (!setIntegrationTimeALS(100))
        return false;

    return true;
}

bool VL6180_7Semi::defaultSetting()
{
    // Load mandatory private register configuration
    if (!bus->write(0x0207, (uint8_t)0x01))
        return false;

    // Load mandatory private register configuration
    if (!bus->write(0x0208, (uint8_t)0x01))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x0096, (uint8_t)0x00))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x0097, (uint8_t)0xFD))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00E3, (uint8_t)0x00))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00E4, (uint8_t)0x04))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00E5, (uint8_t)0x02))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00E6, (uint8_t)0x01))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00E7, (uint8_t)0x03))
        return false;

    // Configure internal ranging calibration
    if (!bus->write(0x00F5, (uint8_t)0x02))
        return false;

    // Configure internal ranging timing
    if (!bus->write(0x00D9, (uint8_t)0x05))
        return false;

    // Configure internal ranging timing
    if (!bus->write(0x00DB, (uint8_t)0xCE))
        return false;

    // Configure internal ranging timing
    if (!bus->write(0x00DC, (uint8_t)0x03))
        return false;

    // Configure internal ranging timing
    if (!bus->write(0x00DD, (uint8_t)0xF8))
        return false;

    // Configure ranging performance
    if (!bus->write(0x009F, (uint8_t)0x00))
        return false;

    // Configure ranging performance
    if (!bus->write(0x00A3, (uint8_t)0x3C))
        return false;

    // Configure ranging performance
    if (!bus->write(0x00B7, (uint8_t)0x00))
        return false;

    // Configure ranging performance
    if (!bus->write(0x00BB, (uint8_t)0x3C))
        return false;

    // Configure ranging performance
    if (!bus->write(0x00B2, (uint8_t)0x09))
        return false;

    // Configure ranging performance
    if (!bus->write(0x00CA, (uint8_t)0x09))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x0198, (uint8_t)0x01))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x01B0, (uint8_t)0x17))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x01AD, (uint8_t)0x00))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x00FF, (uint8_t)0x05))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x0100, (uint8_t)0x05))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x0199, (uint8_t)0x05))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x01A6, (uint8_t)0x1B))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x01AC, (uint8_t)0x3E))
        return false;

    // Configure ALS calibration
    if (!bus->write(0x01A7, (uint8_t)0x1F))
        return false;

    // Final startup configuration
    if (!bus->write(0x0030, (uint8_t)0x00))
        return false;

    return true;
}

bool VL6180_7Semi::getModelId(uint8_t &id)
{
    if (!bus)
        return false;

    // Read model ID register
    if (!bus->read(VL6180_REG_IDENTIFICATION_MODEL_ID, id))
        return false;

    return true;
}

bool VL6180_7Semi::getSensorInfo(float &modelRev, float &moduleRev)
{
    if (!bus)
        return false;

    uint8_t data[4];

    // Read revision information
    if (!bus->read(VL6180_REG_IDENTIFICATION_MODEL_REV_MAJOR, data, 4))
        return false;

    // Convert model revision to floating-point value
    modelRev = data[0] + (data[1] / 10.0f);

    // Convert module revision to floating-point value
    moduleRev = data[2] + (data[3] / 10.0f);

    return true;
}

bool VL6180_7Semi::getManufacturerInfo(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &phase, uint32_t &timeSeconds)
{
    if (!bus)
        return false;

    uint8_t data[4];

    // Read manufacturing information registers
    if (!bus->read(VL6180_REG_IDENTIFICATION_DATE_HI, data, 4))
        return false;

    // Decode manufacturing year
    year = 2000 + ((data[0] >> 4) & 0x0F);

    // Decode manufacturing month
    month = data[0] & 0x0F;

    // Decode manufacturing day
    day = (data[1] >> 3) & 0x1F;

    // Decode manufacturing phase
    phase = data[1] & 0x07;

    // Decode manufacturing time
    uint16_t rawTime = (data[2] << 8) | data[3];

    // Convert to seconds (2-second resolution)
    timeSeconds = rawTime * 2;

    return true;
}

bool VL6180_7Semi::setGPIO0(VL6180_Measurement_Mode mode, bool interrupt, VL6180_GPIO0_Mode polarity)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Configure GPIO0 as XSHUT pin
    v |= ((uint8_t)mode << 6);

    // Configure interrupt polarity
    v |= ((uint8_t)polarity << 5);

    // Enable interrupt output
    v |= (interrupt << 4);

    // Write GPIO0 configuration
    if (!bus->write(VL6180_REG_SYSTEM_MODE_GPIO0, v))
        return false;

    return true;
}

bool VL6180_7Semi::getGPIO0(bool &asXshut, bool &interrupt, bool &activeHigh)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Read GPIO0 configuration register
    if (!bus->read(VL6180_REG_SYSTEM_MODE_GPIO0, v))
        return false;

    // Decode XSHUT configuration
    asXshut = (v & 0x40) != 0;

    // Decode interrupt polarity
    activeHigh = (v & 0x20) != 0;

    // Decode interrupt enable
    interrupt = (v & 0x10) != 0;

    return true;
}

bool VL6180_7Semi::setGPIO1(bool interrupt, bool activeHigh)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Configure interrupt polarity
    v |= (activeHigh << 5);

    // Enable interrupt output
    v |= (interrupt << 4);

    // Write GPIO1 configuration
    if (!bus->write(VL6180_REG_SYSTEM_MODE_GPIO1, v))
        return false;

    return true;
}

bool VL6180_7Semi::getGPIO1(
    bool &interrupt,
    bool &activeHigh)
{
    // Verify bus is initialized
    if (!bus)
        return false;

    uint8_t v = 0;

    // Read GPIO1 configuration register
    if (!bus->read(VL6180_REG_SYSTEM_MODE_GPIO1, v))
        return false;

    // Decode interrupt polarity
    activeHigh = (v & 0x20) != 0;

    // Decode interrupt enable
    interrupt = (v & 0x10) != 0;

    return true;
}

bool VL6180_7Semi::clearHistory()
{
    if (!bus)
        return false;

    // Trigger history buffer clear
    if (!bus->writeBit(VL6180_REG_SYSTEM_HISTORY_CTRL, 2, (uint8_t)0x01))
        return false;

    return true;
}

bool VL6180_7Semi::setHistoryConfig(bool enableBuffing, VL6180_HistoryMode bufferMode)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Configure history buffering enable
    v |= enableBuffing << 1;

    // Configure history storage mode
    v |= (uint8_t)bufferMode;

    // Write history configuration
    if (!bus->write(VL6180_REG_SYSTEM_HISTORY_CTRL, v))
        return false;

    return true;
}

bool VL6180_7Semi::getHistoryConfig(bool &enableBuffing, bool &bufferMode)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Read history configuration register
    if (!bus->read(VL6180_REG_SYSTEM_HISTORY_CTRL, v))
        return false;

    // Decode buffering enable state
    enableBuffing = (v & 0x02) != 0;

    // Decode history storage mode
    bufferMode = (v & 0x01) != 0;

    return true;
}

bool VL6180_7Semi::setInterruptConfigGPIO(VL6180_Interrupt_Mode alsMode, VL6180_Interrupt_Mode rangeMode)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Configure ALS interrupt mode
    v |= ((uint8_t)alsMode & 0x07) << 3;

    // Configure range interrupt mode
    v |= ((uint8_t)rangeMode & 0x07);

    // Write interrupt configuration
    return bus->write(VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, v);
}

bool VL6180_7Semi::getInterruptConfigGPIO(
    uint8_t &alsMode,
    uint8_t &rangeMode)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Read interrupt configuration register
    if (!bus->read(VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, v))
        return false;

    // Decode ALS interrupt mode
    alsMode = (v >> 3) & 0x07;

    // Decode range interrupt mode
    rangeMode = v & 0x07;

    return true;
}

bool VL6180_7Semi::clearRangeInterrupt()
{
    if (!bus)
        return false;

    // Clear range interrupt flag
    if (!bus->writeBit(VL6180_REG_SYSTEM_INTERRUPT_CLEAR, 0, (uint8_t)0x01))
        return false;

    return true;
}

bool VL6180_7Semi::clearALSInterrupt()
{
    if (!bus)
        return false;

    // Clear ALS interrupt flag
    if (!bus->writeBit(VL6180_REG_SYSTEM_INTERRUPT_CLEAR, 1, (uint8_t)0x01))
        return false;

    return true;
}

bool VL6180_7Semi::clearErrorInterrupt()
{
    if (!bus)
        return false;

    // Clear error interrupt flag
    if (!bus->writeBit(VL6180_REG_SYSTEM_INTERRUPT_CLEAR, 2, (uint8_t)0x01))
        return false;

    return true;
}

bool VL6180_7Semi::getInterruptStatus(uint8_t &status)
{
    if (!bus)
        return false;

    // Read interrupt status register
    return bus->read(        VL6180_REG_RESULT_INTERRUPT_STATUS_GPIO,        status);
}

bool VL6180_7Semi::getDistance(uint8_t &distance)
{
    uint8_t rangeStatus;

    return getDistance(distance, rangeStatus);
}

bool VL6180_7Semi::getDistance(uint8_t &distance, uint8_t &rangeStatus)
{
    if (!bus)
        return false;

    // Clear range interrupt flag
    clearRangeInterrupt();

    // Start single-shot ranging measurement
    if (!bus->write(VL6180_REG_SYSRANGE_START, (uint8_t)0x01))
        return false;

    // Wait for range measurement completion
    isRangeReady();

    // Read measured distance
    if (!readDistance(distance))
        return false;

    // Read range status register
    if (!getRangeStatus(rangeStatus))
        return false;

    // Extract error status bits
    rangeStatus = (rangeStatus >> 4) & 0x0F;

    // Status 0 indicates successful measurement
    if (rangeStatus != 0)
        return false;

    return true;
}

bool VL6180_7Semi::startContinuous()
{
    if (!bus)
        return false;

    // Enable continuous ranging mode
    return bus->write(VL6180_REG_SYSRANGE_START, (uint8_t)0x03);
}

bool VL6180_7Semi::stopContinuous()
{
    if (!bus)
        return false;

    // Disable ranging operation
    return bus->writeBit(VL6180_REG_SYSRANGE_START, 0, (uint8_t)0x00);
}

bool VL6180_7Semi::readDistance(uint8_t &distance)
{
    // Read range result register
    if (!bus->read(VL6180_REG_RESULT_RANGE_VAL, distance))
        return false;

    return true;
}

bool VL6180_7Semi::readContinuousDistance(uint8_t &distance)
{
    if (!bus)
        return false;

    // Wait for new ranging measurement
    if(!isRangeReady())
        return false;

    uint8_t rangeStatus = 0;

     // Read range status register
    if (!getRangeStatus(rangeStatus))
        return false;

    // Extract error status bits
    rangeStatus = (rangeStatus >> 4) & 0x0F;

    // Status 0 indicates successful measurement
    if (rangeStatus != 0)
        return false;

    // Read measured distance
    if (!readDistance(distance))
        return false;

    // Clear range interrupt flag
    if (!clearRangeInterrupt())
        return false;

    return true;
}

bool VL6180_7Semi::getAmbientLight(float &lux)
{
    uint8_t alsStatus;

    return getAmbientLight(lux, alsStatus);
}

bool VL6180_7Semi::getAmbientLight(
    float &lux,
    uint8_t &alsStatus)
{
    // Verify bus is initialized
    if (!bus)
        return false;

    // Start single-shot ALS measurement
    if (!bus->write(VL6180_REG_SYSALS_START, (uint8_t)0x01))
        return false;

    // Wait for ALS measurement completion
    isAlsReady();

    // Convert raw ALS measurement to lux
    if (!getLux(lux))
        return false;

    // Read ALS status register
    if (!getAlsStatus(alsStatus))
        return false;

    // Clear ALS interrupt flag
    // if (!clearALSInterrupt())
    //     return false;

    // Extract ALS error status bits
    alsStatus = (alsStatus >> 4) & 0x0F;

    // Status 0 indicates successful measurement
    if (alsStatus != 0)
        return false;

    return true;
}

bool VL6180_7Semi::readAls(uint16_t &als)
{
    uint8_t data[2];

    // Read raw ALS result registers
    if (!bus->read(VL6180_REG_RESULT_ALS_VAL, data, 2))
        return false;

    // Combine MSB and LSB into 16-bit value
    als = ((uint16_t)data[0] << 8) | data[1];

    return true;
}

bool VL6180_7Semi::startContinuousALS()
{
    if (!bus)
        return false;

    // Enable continuous ALS mode
    return bus->write(VL6180_REG_SYSALS_START, (uint8_t)0x03);
}

bool VL6180_7Semi::stopContinuousALS()
{
    if (!bus)
        return false;

    // Disable ALS operation
    return bus->writeBit(VL6180_REG_SYSALS_START, 0, (uint8_t)0x00);
}

bool VL6180_7Semi::readContinuousALS(float &lux)
{
    if (!bus)
        return false;

    // Wait for ALS measurement completion
    isAlsReady();

    // Convert raw ALS value to lux
    if (!getLux(lux))
        return false;

    uint8_t alsStatus;

    // Read ALS status register
    if (!getAlsStatus(alsStatus))
        return false;

    // Clear ALS interrupt flag
    if (!clearALSInterrupt())
        return false;

    // Extract ALS error status bits
    alsStatus = (alsStatus >> 4) & 0x0F;

    // Status 0 indicates successful measurement
    if (alsStatus != 0)
        return false;

    return true;
}

bool VL6180_7Semi::getLux(float &lux)
{
    if (!bus)
        return false;

    uint16_t als;

    // Read raw ALS measurement
    if (!readAls(als))
        return false;

    // Convert raw ALS count to lux
    lux = als * 0.32f * 100.0f / als_integration_time;

    // Apply ALS gain compensation
    switch (als_gain)
    {
    case VL6180_ALS_Gain::VL6180_ALS_GAIN_1:
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_1_25:
        lux /= 1.25f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_1_67:
        lux /= 1.67f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_2_5:
        lux /= 2.50f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_5:
        lux /= 5.00f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_10:
        lux /= 10.00f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_20:
        lux /= 20.00f;
        break;

    case VL6180_ALS_Gain::VL6180_ALS_GAIN_40:
        lux /= 40.00f;
        break;
    }

    return true;
}

bool VL6180_7Semi::isRangeReady()
{
    uint8_t status = 0;

    // Wait for range measurement completion
    for (uint16_t i = 0; i < 300; i++)
    {
        // Read interrupt status register
        if (getInterruptStatus(status))
        {
            // Check for new range sample ready event
            if ((status & 0x07) == 0x04)
                return true;
        }

        delay(1);
    }

    // Timeout
    return false;
}

bool VL6180_7Semi::isAlsReady()
{
    uint8_t status = 0;

    // Wait for ALS measurement completion
    for (uint16_t i = 0; i < 300; i++)
    {
        // Read interrupt status register
        if (getInterruptStatus(status))
        {
            // Check for new ALS sample ready event
            if (((status >> 3) & 0x07) == 0x04)
                return true;
        }

        delay(1);
    }

    // Timeout
    return false;
}

bool VL6180_7Semi::getRangeStatus(uint8_t &status)
{
    if (!bus)
        return false;

    // Read range status register
    return bus->read(VL6180_REG_RESULT_RANGE_STATUS, status);
}

bool VL6180_7Semi::getAlsStatus(uint8_t &status)
{
    if (!bus)
        return false;

    // Read ALS status register
    return bus->read(VL6180_REG_RESULT_ALS_STATUS, status);
}

bool VL6180_7Semi::setIntegrationTimeALS(uint16_t integrationTime)
{
    if (!bus)
        return false;

    // Enforce minimum integration time
    if (integrationTime < 1)
        integrationTime = 1;

    // Convert milliseconds to register format
    uint16_t regValue = integrationTime - 1;

    uint8_t data[2];

    // Store MSB
    data[0] = regValue >> 8;

    // Store LSB
    data[1] = regValue & 0xFF;

    // Write integration period register
    if (!bus->write(VL6180_REG_SYSALS_INTEGRATION_PERIOD, data, 2))
        return false;

    // Cache integration time
    als_integration_time = integrationTime;

    return true;
}

bool VL6180_7Semi::getIntegrationTimeALS(uint16_t &integrationTime)
{
    if (!bus)
        return false;

    uint8_t data[2];

    // Read integration period register
    if (!bus->read(VL6180_REG_SYSALS_INTEGRATION_PERIOD, data, 2))
        return false;

    // Combine MSB and LSB
    uint16_t regValue = ((uint16_t)data[0] << 8) | data[1];

    // Convert register value to milliseconds
    integrationTime = regValue + 1;

    return true;
}

bool VL6180_7Semi::setMeasurementInterval(uint16_t period)
{
    if (!bus)
        return false;

    if (period > 2550)
        return false;

    // Enforce minimum interval
    if (period < 10)
        period = 10;

    // Convert milliseconds to register format
    uint16_t v = (period / 10) - 1;

    // Write intermeasurement period register
    return bus->write(VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, (uint8_t)v);
}

bool VL6180_7Semi::getMeasurementInterval(uint16_t &period)
{
    if (!bus)
        return false;

    uint8_t v = 0;

    // Read intermeasurement period register
    if (!bus->read(VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, v))
        return false;

    // Convert register value to milliseconds
    period = (v + 1) * 10;

    return true;
}

bool VL6180_7Semi::setThreshold(uint8_t lowTh, uint8_t highTh)
{
    if (!bus)
        return false;

    // Configure high threshold
    if (!bus->write(VL6180_REG_SYSRANGE_THRESH_HIGH, highTh))
        return false;

    // Configure low threshold
    if (!bus->write(VL6180_REG_SYSRANGE_THRESH_LOW, lowTh))
        return false;

    return true;
}

bool VL6180_7Semi::getThreshold(uint8_t &lowTh, uint8_t &highTh)
{
    if (!bus)
        return false;

    // Read high threshold
    if (!bus->read(VL6180_REG_SYSRANGE_THRESH_HIGH, highTh))
        return false;

    // Read low threshold
    if (!bus->read(VL6180_REG_SYSRANGE_THRESH_LOW, lowTh))
        return false;

    return true;
}

bool VL6180_7Semi::setAlsGain(VL6180_ALS_Gain gain)
{
    if (!bus)
        return false;

    // Configure ALS gain register
    if (!bus->write(VL6180_REG_SYSALS_ANALOGUE_GAIN, 0x40 | (uint8_t)gain))
        return false;

    // Current gain setting
    als_gain = gain;

    return true;
}

bool VL6180_7Semi::getAlsGain(uint8_t &gain)
{
    if (!bus)
        return false;

    // Read ALS gain bits
    if (!bus->readBits(
            VL6180_REG_SYSALS_ANALOGUE_GAIN, 0, 3, gain))
        return false;

    return true;
}

bool VL6180_7Semi::setMaxConvergenceTime(uint8_t ms)
{
    if (!bus)
        return false;

    // Enforce minimum convergence time
    if (ms == 0)
        ms = 1;

    if(ms > 63)
        return false;

    // Write convergence time register
    return bus->write(VL6180_REG_SYSRANGE_MAX_CONVERGENCE_TIME, ms);
}

bool VL6180_7Semi::getMaxConvergenceTime(uint8_t &ms)
{
    if (!bus)
        return false;

    // Read convergence time register
    return bus->read(VL6180_REG_SYSRANGE_MAX_CONVERGENCE_TIME, ms);
}

bool VL6180_7Semi::setI2CAddress(uint8_t address)
{
    if (!bus)
        return false;

    // Reject reserved I2C addresses
    if (address < 0x08)
        return false;

    // Verify 7-bit address range
    if (address > 0x7F)
        return false;

    // Update sensor I2C address
    if (!bus->write(VL6180_REG_I2C_SLAVE_DEVICE_ADDRESS, address))
        return false;

    // Update interface address
    i2c.setAddress(address);

    return true;
}

bool VL6180_7Semi::getI2CAddress(uint8_t &address)
{
    if (!bus)
        return false;

    // Read current I2C address
    if (!bus->read(VL6180_REG_I2C_SLAVE_DEVICE_ADDRESS, address))
        return false;

    return true;
}
