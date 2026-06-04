/**
 * 7Semi BMM150 Library
 *
 * - High-performance driver for Bosch BMM150 magnetometer
 * - Supports both I2C and SPI communication
 *
 * Features:
 * - Full sensor configuration support
 * - Built-in compensation algorithms
 * - Interrupt and threshold handling
 *
 * Notes:
 * - Calibration data is read internally
 * - Output values are compensated
 *
 * License:
 * - MIT License (7Semi)
 */

#pragma once
#include <Arduino.h>
#include "7Semi_Interface.h"
#include "7Semi_I2C.h"
#include "7Semi_Bus.h"

#define VL6180_IDENTIFICATION_MODEL_ID                         0xB4

#define VL6180_REG_IDENTIFICATION_MODEL_ID                     0x000
#define VL6180_REG_IDENTIFICATION_MODEL_REV_MAJOR              0x001
#define VL6180_REG_IDENTIFICATION_MODEL_REV_MINOR              0x002
#define VL6180_REG_IDENTIFICATION_MODULE_REV_MAJOR             0x003
#define VL6180_REG_IDENTIFICATION_MODULE_REV_MINOR             0x004
#define VL6180_REG_IDENTIFICATION_DATE_HI                      0x006
#define VL6180_REG_IDENTIFICATION_DATE_LO                      0x007
#define VL6180_REG_IDENTIFICATION_TIME                         0x008  // (0x008–0x009)

#define VL6180_REG_SYSTEM_MODE_GPIO0                           0x010
#define VL6180_REG_SYSTEM_MODE_GPIO1                           0x011
#define VL6180_REG_SYSTEM_HISTORY_CTRL                         0x012
#define VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO                0x014
#define VL6180_REG_SYSTEM_INTERRUPT_CLEAR                      0x015
#define VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET                   0x016
#define VL6180_REG_SYSTEM_GROUPED_PARAMETER_HOLD               0x017

#define VL6180_REG_SYSRANGE_START                             0x018
#define VL6180_REG_SYSRANGE_THRESH_HIGH                       0x019
#define VL6180_REG_SYSRANGE_THRESH_LOW                        0x01A
#define VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD           0x01B
#define VL6180_REG_SYSRANGE_MAX_CONVERGENCE_TIME              0x01C
#define VL6180_REG_SYSRANGE_CROSSTALK_COMPENSATION_RATE       0x01E
#define VL6180_REG_SYSRANGE_CROSSTALK_VALID_HEIGHT            0x021
#define VL6180_REG_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE        0x022
#define VL6180_REG_SYSRANGE_PART_TO_PART_RANGE_OFFSET         0x024
#define VL6180_REG_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT         0x025
#define VL6180_REG_SYSRANGE_RANGE_IGNORE_THRESHOLD            0x026
#define VL6180_REG_SYSRANGE_MAX_AMBIENT_LEVEL_MULT            0x02C
#define VL6180_REG_SYSRANGE_RANGE_CHECK_ENABLES               0x02D
#define VL6180_REG_SYSRANGE_VHV_RECALIBRATE                   0x02E
#define VL6180_REG_SYSRANGE_VHV_REPEAT_RATE                   0x031

#define VL6180_REG_SYSALS_START                               0x038
#define VL6180_REG_SYSALS_THRESH_HIGH                         0x03A
#define VL6180_REG_SYSALS_THRESH_LOW                          0x03C
#define VL6180_REG_SYSALS_INTERMEASUREMENT_PERIOD             0x03E
#define VL6180_REG_SYSALS_ANALOGUE_GAIN                       0x03F
#define VL6180_REG_SYSALS_INTEGRATION_PERIOD                  0x040
#define VL6180_REG_RESULT_RANGE_STATUS                        0x04D
#define VL6180_REG_RESULT_ALS_STATUS                          0x04E
#define VL6180_REG_RESULT_INTERRUPT_STATUS_GPIO               0x04F
#define VL6180_REG_RESULT_ALS_VAL                             0x050
#define VL6180_REG_RESULT_HISTORY_BUFFER_0                    0x052  // (0x052–0x060)
#define VL6180_REG_RESULT_RANGE_VAL                           0x062

#define VL6180_REG_RESULT_RANGE_RAW                      0x064
#define VL6180_REG_RESULT_RANGE_RETURN_RATE              0x066
#define VL6180_REG_RESULT_RANGE_REFERENCE_RATE           0x068
#define VL6180_REG_RESULT_RANGE_RETURN_SIGNAL_COUNT      0x06C
#define VL6180_REG_RESULT_RANGE_REFERENCE_SIGNAL_COUNT   0x070
#define VL6180_REG_RESULT_RANGE_RETURN_AMB_COUNT         0x074
#define VL6180_REG_RESULT_RANGE_REFERENCE_AMB_COUNT      0x078
#define VL6180_REG_RESULT_RANGE_RETURN_CONV_TIME         0x07C
#define VL6180_REG_RESULT_RANGE_REFERENCE_CONV_TIME      0x080

#define VL6180_REG_READOUT_AVERAGING_SAMPLE_PERIOD       0x10A
#define VL6180_REG_FIRMWARE_BOOTUP                       0x119
#define VL6180_REG_I2C_SLAVE_DEVICE_ADDRESS              0x212


/**
 * History buffer storage mode
 *
 * - Selects which measurement type is stored
 * - History buffer stores up to 8 samples
 */
enum class VL6180_HistoryMode : uint8_t
{
    Ranging = 0, // Store last 8 ranging measurements
    ALS     = 1  // Store last 8 ALS measurements
};

/**
 * Interrupt trigger mode
 *
 * - Controls when GPIO interrupt is generated
 */
enum class VL6180_Interrupt_Mode : uint8_t
{
    INT_DISABLED         = 0, // Interrupt disabled
    INT_LEVEL_LOW        = 1, // Value below low threshold
    INT_LEVEL_HIGH       = 2, // Value above high threshold
    INT_OUT_OF_WINDOW    = 3, // Value outside threshold window
    INT_NEW_SAMPLE_READY = 4  // New measurement available
};

/**
 * ALS analog gain setting
 *
 * - Controls ambient light sensor sensitivity
 */
enum class VL6180_ALS_Gain : uint8_t
{
    VL6180_ALS_GAIN_20   = 0,
    VL6180_ALS_GAIN_10   = 1,
    VL6180_ALS_GAIN_5    = 2,
    VL6180_ALS_GAIN_2_5  = 3,
    VL6180_ALS_GAIN_1_67 = 4,
    VL6180_ALS_GAIN_1_25 = 5,
    VL6180_ALS_GAIN_1    = 6,
    VL6180_ALS_GAIN_40   = 7
};

/**
 * Measurement operating mode
 */
enum class VL6180_Measurement_Mode : uint8_t
{
    SINGLE_SHOT_MODE = 0,
    CONTINUOUS_MODE  = 1
};

/**
 * GPIO interrupt polarity
 */
enum class VL6180_GPIO_Polarity : uint8_t
{
    ACTIVE_LOW  = 0,
    ACTIVE_HIGH = 1
};

/**
 * GPIO0 operating mode
 */
enum class VL6180_GPIO0_Mode : uint8_t
{
    GPIO  = 0,
    XSHUT = 1
};

class VL6180_7Semi
{
public:
    VL6180_7Semi();

    ~VL6180_7Semi();

    /**
     * begin()
     *
     * - Initializes VL6180X over I2C
     * - Creates BusCore communication interface
     * - Verifies sensor model ID
     * - Applies ST recommended startup configuration
     * - Configures ranging and ALS defaults
     *
     * Default Configuration:
     * - Range interrupt: New sample ready
     * - Range intermeasurement period: 100ms
     * - ALS gain: 1x
     * - ALS integration time: 100ms
     * - VHV repeat rate: 255
     *
     * Notes:
     * - Can be called multiple times safely
     * - Existing bus instance is released before reinitialization
     * - Returns false if sensor is not detected
     */
    bool begin(uint8_t addr = 0x29, TwoWire &wire = Wire, uint32_t speed = 400000);

    /**
     * getModelId()
     *
     * - Reads VL6180X model identification register
     * - Used to verify sensor presence and communication
     *
     * Output:
     * - id → Sensor model ID
     *
     * Notes:
     * - Expected value: 0xB4
     */
    bool getModelId(uint8_t &id);

    /**
     * getSensorInfo()
     *
     * - Reads sensor revision information
     * - Returns model and module revision numbers
     *
     * Output:
     * - modelRev  → Sensor silicon revision
     * - moduleRev → Module hardware revision
     *
     * Notes:
     * - Values are converted to floating-point format
     * - Example: 1.0, 1.1, 2.0
     */
    bool getSensorInfo(float &modelRev, float &moduleRev);

    /**
     * getManufacturerInfo()
     *
     * - Reads manufacturing information
     * - Returns production date and manufacturing phase
     *
     * Output:
     * - year        → Manufacturing year
     * - month       → Manufacturing month
     * - day         → Manufacturing day
     * - phase       → Manufacturing phase code
     * - timeSeconds → Manufacturing time in seconds
     *
     * Notes:
     * - Date is encoded by ST in identification registers
     * - Time value has 2-second resolution
     */
    bool getManufacturerInfo(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &phase, uint32_t &timeSeconds);

    /**
     * setGPIO0()
     *
     * - Configures GPIO0 pin behavior
     * - Selects XSHUT or interrupt mode
     * - Configures interrupt polarity
     *
     * Input:
     * - asXshut    → True = XSHUT, False = GPIO
     * - interrupt  → Enable interrupt output
     * - activeHigh → True = Active high, False = Active low
     *
     * Notes:
     * - GPIO0 can be used as hardware shutdown pin
     * - Polarity only affects interrupt mode
     */
    bool setGPIO0(VL6180_Measurement_Mode mode, bool interrupt, VL6180_GPIO0_Mode polarity);

    /**
     * getGPIO0()
     *
     * - Reads GPIO0 configuration
     * - Returns XSHUT mode, interrupt state and polarity
     *
     * Output:
     * - asXshut    → True = XSHUT, False = GPIO
     * - interrupt  → Interrupt output enabled
     * - activeHigh → Interrupt polarity
     */
    bool getGPIO0(bool &asXshut, bool &interrupt, bool &activeHigh);

    /**
     * setGPIO1()
     *
     * - Configures GPIO1 interrupt output
     * - Configures interrupt polarity
     *
     * Input:
     * - interrupt  → Enable interrupt output
     * - activeHigh → True = Active high, False = Active low
     *
     * Notes:
     * - GPIO1 is dedicated for interrupt output
     */
    bool setGPIO1(bool interrupt, bool activeHigh);

    /**
     * getGPIO1()
     *
     * - Reads GPIO1 configuration
     * - Returns interrupt enable state and polarity
     *
     * Output:
     * - interrupt  → Interrupt output enabled
     * - activeHigh → Interrupt polarity
     */
    bool getGPIO1(bool &interrupt, bool &activeHigh);

    /**
     * setInterruptConfigGPIO()
     *
     * - Configures ALS and ranging interrupt conditions
     * - Selects when interrupt events are generated
     *
     * Input:
     * - alsMode   → ALS interrupt trigger mode
     * - rangeMode → Range interrupt trigger mode
     *
     * Notes:
     * - Interrupt polarity is configured separately
     */
    bool setInterruptConfigGPIO(VL6180_Interrupt_Mode alsMode, VL6180_Interrupt_Mode rangeMode);

    /**
     * getInterruptConfigGPIO()
     *
     * - Reads ALS and ranging interrupt configuration
     * - Returns configured interrupt trigger modes
     *
     * Output:
     * - alsMode   → ALS interrupt mode
     * - rangeMode → Range interrupt mode
     *
     * Notes:
     * - Values correspond to VL6180_Interrupt_Mode
     */
    bool getInterruptConfigGPIO(uint8_t &alsMode, uint8_t &rangeMode);

    /**
     * clearRangeInterrupt()
     *
     * - Clears range measurement interrupt flag
     * - Allows next range interrupt event to occur
     *
     * Notes:
     * - Acknowledges completed range measurement
     * - Does not affect ALS interrupt status
     */
    bool clearRangeInterrupt();

    /**
     * clearALSInterrupt()
     *
     * - Clears ambient light interrupt flag
     * - Allows next ALS interrupt event to occur
     *
     * Notes:
     * - Acknowledges completed ALS measurement
     * - Does not affect range interrupt status
     */
    bool clearALSInterrupt();

    /**
     * clearErrorInterrupt()
     *
     * - Clears sensor error interrupt flag
     * - Acknowledges reported error condition
     *
     * Notes:
     * - Used after handling ranging or ALS errors
     * - Does not clear range or ALS interrupts
     */
    bool clearErrorInterrupt();

    /**
     * getInterruptStatus()
     *
     * - Reads current interrupt status register
     * - Returns ALS and ranging interrupt states
     * - Indicates measurement completion events
     *
     * Output:
     * - status → Raw interrupt status register value
     *
     * Interrupt Status Codes:
     *
     * - 0 = No interrupt
     * - 1 = Low threshold
     * - 2 = High threshold
     * - 3 = Out of window
     * - 4 = New sample ready
     *
     * Notes:
     * - Bits [2:0] represent range interrupt status
     * - Bits [5:3] represent ALS interrupt status
     */
    bool getInterruptStatus(uint8_t &status);

    /**
     * clearHistory()
     *
     * - Clears internal history buffer
     * - Removes previously stored ranging or ALS samples
     *
     * Notes:
     * - Sets SYSTEM__HISTORY_CTRL clear bit
     * - History storage mode is preserved
     */
    bool clearHistory();

    /**
     * setHistoryConfig()
     *
     * - Configures history buffer operation
     * - Selects ranging or ALS history storage
     * - Enables or disables buffering
     *
     * Input:
     * - enableBuffing → Enable history storage
     * - bufferMode    → Ranging or ALS history mode
     *
     * Notes:
     * - Ranging mode stores distance samples
     * - ALS mode stores ambient light samples
     */
    bool setHistoryConfig(bool enableBuffing, VL6180_HistoryMode bufferMode);

    /**
     * getHistoryConfig()
     *
     * - Reads history buffer configuration
     * - Returns buffering state and storage mode
     *
     * Output:
     * - enableBuffing → History buffering enabled
     * - bufferMode    → False = Ranging, True = ALS
     *
     * Notes:
     * - Corresponds to SYSTEM__HISTORY_CTRL register
     */
    bool getHistoryConfig(bool &enableBuffing, bool &bufferMode);

    /**
     * getDistance()
     *
     * - Performs a single-shot ranging measurement
     * - Returns measured distance in millimeters
     *
     * Output:
     * - distance → Measured distance value
     */
    bool getDistance(uint8_t &distance);

    /**
     * getDistance()
     *
     * - Performs a single-shot ranging measurement
     * - Starts a new range conversion
     * - Waits for measurement completion
     * - Returns measured distance and status
     *
     * Output:
     * - distance    → Measured distance in millimeters
     * - rangeStatus → Range error status code
     *
     * Notes:
     * - Range interrupt is cleared automatically
     * - Status code 0 indicates valid measurement
     * - Non-zero status indicates ranging error
     */
    bool getDistance(uint8_t &distance, uint8_t &rangeStatus);

    /**
     * startContinuous()
     *
     * - Starts continuous ranging mode
     * - Sensor performs measurements automatically
     * - Results become available at configured interval
     *
     * Notes:
     * - Measurement interval is configured using
     *   setMeasurementInterval()
     * - Use readDistance() to retrieve measurements
     * - Use stopContinuous() to stop ranging
     */
    bool startContinuous();

    /**
     * stopContinuous()
     *
     * - Stops continuous ranging mode
     * - Sensor returns to idle state
     *
     * Notes:
     * - Ongoing measurement is terminated
     */
    bool stopContinuous();

    /**
     * readContinuousDistance()
     *
     * - Reads latest distance measurement
     * - Intended for continuous ranging mode
     * - Does not start a new ranging conversion
     *
     * Output:
     * - distance → Measured distance in millimeters
     *
     * Notes:
     * - Range interrupt is cleared automatically
     * - Measurement must already be available
     * - Use startContinuous() before calling
     */
    bool readContinuousDistance(uint8_t &distance);

    /**
     * getAmbientLight()
     *
     * - Performs a single-shot ambient light measurement
     * - Returns calculated ambient light in lux
     *
     * Output:
     * - lux → Ambient light level in lux
     */
    bool getAmbientLight(float &lux);

    /**
     * getAmbientLight()
     *
     * - Performs a single-shot ambient light measurement
     * - Starts a new ALS conversion
     * - Waits for measurement completion
     * - Converts raw ALS data to lux
     *
     * Output:
     * - lux       → Ambient light level in lux
     * - alsStatus → ALS status code
     *
     * Notes:
     * - ALS interrupt is cleared automatically
     * - Status code 0 indicates valid measurement
     * - Non-zero status indicates ALS measurement error
     */
    bool getAmbientLight(float &lux, uint8_t &luxStatus);

    /**
     * startContinuousALS()
     *
     * - Starts continuous ambient light measurements
     * - Sensor performs ALS conversions automatically
     * - Results are updated at the configured ALS interval
     *
     * Notes:
     * - Use readContinuousALS() to read measurements
     * - Use stopContinuousALS() to stop operation
     * - ALS integration time affects measurement speed
     */
    bool startContinuousALS();

    /**
     * stopContinuousALS()
     *
     * - Stops continuous ambient light measurements
     * - Sensor returns to idle state
     *
     * Notes:
     * - Does not clear pending ALS interrupts
     * - Does not modify ALS configuration settings
     */
    bool stopContinuousALS();

    /**
     * readContinuousALS()
     *
     * - Reads latest ambient light measurement
     * - Intended for continuous ALS mode
     * - Does not start a new ALS conversion
     *
     * Output:
     * - lux → Ambient light level in lux
     *
     * Notes:
     * - ALS interrupt is cleared automatically
     * - Measurement must already be available
     * - Use startContinuousALS() before calling
     */
    bool readContinuousALS(float &lux);

    /**
     * getLux()
     *
     * - Converts raw ALS measurement to lux
     * - Applies integration time compensation
     * - Applies ALS gain compensation
     *
     * Output:
     * - lux → Ambient light level in lux
     *
     * Notes:
     * - Uses cached ALS gain setting
     * - Uses cached integration time setting
     * - Does not start a new measurement
     */
    bool getLux(float &lux);

    /**
     * isRangeReady()
     *
     * - Waits for range measurement completion
     * - Polls interrupt status register
     * - Checks for new range sample availability
     *
     * Output:
     * - True  → New range measurement available
     * - False → Timeout
     *
     * Notes:
     * - Used by single-shot and continuous ranging modes
     */
    bool isRangeReady();

    /**
     * isAlsReady()
     *
     * - Waits for ALS measurement completion
     * - Polls interrupt status register
     * - Checks for new ALS sample availability
     *
     * Output:
     * - True  → New ALS measurement available
     * - False → Timeout
     *
     * Notes:
     * - Used by single-shot and continuous ALS modes
     */
    bool isAlsReady();

    /**
     * getRangeStatus()
     *
     * - Reads range measurement status register
     * - Returns raw ranging status information
     *
     * Output:
     * - status → Raw range status register value
     *
     * Notes:
     * - Value 0 indicates successful measurement
     * - Used to detect convergence and ranging errors
     */
    bool getRangeStatus(uint8_t &status);

    /**
     * getAlsStatus()
     *
     * - Reads ALS measurement status register
     * - Returns raw ALS status information
     *
     * Output:
     * - status → Raw ALS status register value
     *
     * Notes:
     * - Value 0 indicates successful measurement
     * - Used to detect ALS measurement errors
     */
    bool getAlsStatus(uint8_t &status);

    /**
     * setIntegrationTimeALS()
     *
     * - Configures ALS integration time
     * - Controls ambient light accumulation period
     * - Affects ALS sensitivity and update rate
     *
     * Input:
     * - integrationTime → Integration time in milliseconds
     *
     * Notes:
     * - Higher values improve low-light sensitivity
     * - Higher values increase measurement time
     * - Minimum value is 1ms
     * - Recommended setting is 100 ms
     */
    bool setIntegrationTimeALS(uint16_t integrationTime);

    /**
     * getIntegrationTimeALS()
     *
     * - Reads ALS integration time setting
     *
     * Output:
     * - integrationTime → Integration time in milliseconds
     */
    bool getIntegrationTimeALS(uint16_t &integrationTime);

    /**
     * setMeasurementInterval()
     *
     * - Configures continuous ranging measurement interval
     * - Controls time between consecutive range measurements
     *
     * Input:
     * - period → Measurement interval in milliseconds
     *
     * Notes:
     * - Minimum interval is 10ms
     * - Maximum supported interval is 2550ms
     * - Used only in continuous ranging mode
     */
    bool setMeasurementInterval(uint16_t period);

    /**
     * getMeasurementInterval()
     *
     * - Reads continuous ranging measurement interval
     *
     * Output:
     * - period → Measurement interval in milliseconds
     */
    bool getMeasurementInterval(uint16_t &period);

    /**
     * setThreshold()
     *
     * - Configures range interrupt threshold values
     * - Sets low and high distance limits
     * - Used with threshold-based interrupt modes
     *
     * Input:
     * - lowTh  → Low distance threshold in millimeters
     * - highTh → High distance threshold in millimeters
     *
     * Notes:
     * - Threshold values are used by range interrupt logic
     * - Valid range depends on sensor operating range
     */
    bool setThreshold(uint8_t lowTh, uint8_t highTh);

    /**
     * getThreshold()
     *
     * - Reads range interrupt threshold values
     *
     * Output:
     * - lowTh  → Low distance threshold in millimeters
     * - highTh → High distance threshold in millimeters
     */
    bool getThreshold(uint8_t &lowTh, uint8_t &highTh);

    /**
     * setAlsGain()
     *
     * - Configures ALS analog gain
     * - Adjusts ambient light sensor sensitivity
     *
     * Input:
     * - gain → ALS gain setting
     *
     * Notes:
     * - Higher gain improves low-light sensitivity
     * - Higher gain increases saturation risk
     */
    bool setAlsGain(VL6180_ALS_Gain gain);

    /**
     * getAlsGain()
     *
     * - Reads ALS gain configuration
     *
     * Output:
     * - gain → Current ALS gain setting
     *
     * Notes:
     * - Returned value corresponds to VL6180_ALS_Gain
     */
    bool getAlsGain(uint8_t &gain);

    /**
     * setMaxConvergenceTime()
     *
     * - Configures maximum ranging convergence time
     * - Limits how long the sensor searches for a valid target
     *
     * Input:
     * - ms → Maximum convergence time in milliseconds
     *
     * Notes:
     * - Minimum value is 1ms
     * - Maximum value is 63ms
     * - Larger values improve long-range detection
     * - Larger values increase measurement duration
     * - Useful for reducing Max Convergence errors
     */
    bool setMaxConvergenceTime(uint8_t ms);

    /**
     * getMaxConvergenceTime()
     *
     * - Reads maximum ranging convergence time
     *
     * Output:
     * - ms → Maximum convergence time in milliseconds
     */
    bool getMaxConvergenceTime(uint8_t &ms);

    /**
     * setI2CAddress()
     *
     * - Changes sensor I2C slave address
     * - Updates internal I2C interface address
     *
     * Input:
     * - address → New 7-bit I2C address
     *
     * Notes:
     * - Valid address range: 0x08 to 0x7F
     * - Address change is not retained after power cycle
     * - Sensor returns to default address after reset
     */
    bool setI2CAddress(uint8_t address);

    /**
     * getI2CAddress()
     *
     * - Reads current sensor I2C address
     *
     * Output:
     * - address → Current 7-bit I2C address
     */
    bool getI2CAddress(uint8_t &address);

private:
    I2C_7Semi i2c;

    BusIO_7Semi<I2C_7Semi, 2, 1> *bus = nullptr;

    VL6180_ALS_Gain als_gain = VL6180_ALS_Gain::VL6180_ALS_GAIN_1; 

    uint16_t als_integration_time = 100;

    bool readCalibrartionData();

    bool readDistance(uint8_t &distance);

    bool readAls(uint16_t &als);

    bool defaultSetting();
};