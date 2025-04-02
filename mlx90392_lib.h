#ifndef MLX90392_LIB_H
#define MLX90392_LIB_H

#include <Wire.h>
#include <cstdint>

// Define device addresses
#define MLX90392ELQ_AAA_010_RE_ADDR 0x0C
#define MLX90392ELQ_AAA_011_RE_ADDR 0x0C
#define MLX90392ELQ_AAA_013_RE_ADDR 0x3C

class MLX90392 {
public:
    // Constructor with default device address
    MLX90392(uint8_t deviceAddress = MLX90392ELQ_AAA_010_RE_ADDR);

    // Destructor
    virtual ~MLX90392();

    // Initialize the sensor
    bool init();

    // Read magnetic field data (X, Y, Z)
    bool readMagneticField(int16_t data[3]);

    // Read temperature data
    bool readTemperature(int16_t& temperature);

    // Read a specific register from the sensor
    bool readRegister(uint8_t registerAddress, uint8_t& data);

    // Write to a specific register of the sensor
    bool writeRegister(uint8_t registerAddress, uint8_t data);

    // Set the application mode
    enum Mode {
        POWER_DOWN = 0,
        SINGLE_MEASUREMENT = 1,
        CONTINUOUS_10HZ = 2,
        CONTINUOUS_20HZ = 3,
        CONTINUOUS_50HZ = 4,
        CONTINUOUS_100HZ = 5,
        SELF_TEST = 6,
        CONTINUOUS_200HZ = 10,
        CONTINUOUS_500HZ = 11,
        CONTINUOUS_700HZ = 12,
        CONTINUOUS_1_4KHZ = 13
    };
    bool setMode(Mode mode);

    // Get the current sensor mode
    bool getMode(Mode& mode);

    // Read status register 1
    bool readStatus1(uint8_t& status);

    // Read status register 2
    bool readStatus2(uint8_t& status);

    // Check if data is ready
    bool isDataReady();

    // Reset device
    bool deviceReset();

    // Check if device was reset
    bool isDeviceReset();

    // Check if data overrun occurred
    bool isDataOverrun();

    // Check if magnetic sensor overflow occurred
    bool isMagneticSensorOverflow();

    // Perform a single measurement
    bool singleMeasurement(int16_t data[3]);

    // Perform a single measurement and return magnetic field data in microteslas (uT)
    bool singleMeasurement_uT(float data[3]);

    // Get the manufacturer ID
    bool getManufacturerID(uint8_t& manufacturerID);

    // Get the device ID
    bool getDeviceID(uint8_t& deviceID);

    // Set the oversampling ratio for magnetic measurements
    bool setOversamplingRatio(bool ratio);

    // Get the oversampling ratio for magnetic measurements
    bool getOversamplingRatio(bool& ratio);

    // Set the oversampling ratio for temperature measurements
    bool setTemperatureOversamplingRatio(bool ratio);

    // Get the oversampling ratio for temperature measurements
    bool getTemperatureOversamplingRatio(bool& ratio);

    // Set the digital filter for X and Y magnetic measurements
    bool setXYDigitalFilter(uint8_t filter);

    // Get the digital filter for X and Y magnetic measurements
    bool getXYDigitalFilter(uint8_t& filter);

    // Set the digital filter for Z magnetic measurements
    bool setZDigitalFilter(uint8_t filter);

    // Get the digital filter for Z magnetic measurements
    bool getZDigitalFilter(uint8_t& filter);

    // Set the digital filter for temperature measurements
    bool setTemperatureDigitalFilter(uint8_t filter);

    // Get the digital filter for temperature measurements
    bool getTemperatureDigitalFilter(uint8_t& filter);

    // Enable or disable temperature measurement and compensation
    bool setTemperatureCompensation(bool enable);

    // Get the state of temperature measurement and compensation
    bool getTemperatureCompensation(bool& enable);

    // Sensor range enumeration
    enum SensorRange {
        RANGE_5MT = 0,
        RANGE_50MT = 1
    };

    // Set the sensor range
    void setSensorRange(SensorRange range);

    // Get the sensor range
    SensorRange getSensorRange() const;

    // Get magnetic field data in microteslas (uT)
    bool getMagneticField_uT(float data[3]);

    // Perform a self-test and return the difference in magnetic field data
    bool selfTest(int16_t data[3]);

private:
    uint8_t deviceAddress;
    SensorRange sensorRange;

    // Helper function to read multiple bytes
    bool readBytes(uint8_t registerAddress, uint8_t* data, size_t length);

    // Helper function to write multiple bytes
    bool writeBytes(uint8_t registerAddress, const uint8_t* data, size_t length);
};

#endif // MLX90392_LIB_H
