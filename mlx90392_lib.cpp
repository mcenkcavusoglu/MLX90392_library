#include "mlx90392_lib.h"

// Constructor with default device address
MLX90392::MLX90392(uint8_t deviceAddress) : deviceAddress(deviceAddress), sensorRange(RANGE_5MT) {}

// Destructor
MLX90392::~MLX90392() {}

// Initialize the sensor
bool MLX90392::init() {
    Wire.begin();
    // Additional initialization code if needed
    return true;
}

// Read magnetic field data (X, Y, Z)
bool MLX90392::readMagneticField(int16_t data[3]) {
    uint8_t rawData[6];
    if (!readBytes(0x01, rawData, 6)) {
        return false;
    }

    // Convert raw data to 16-bit values
    data[0] = (rawData[1] << 8) | rawData[0]; // X-axis
    data[1] = (rawData[3] << 8) | rawData[2]; // Y-axis
    data[2] = (rawData[5] << 8) | rawData[4]; // Z-axis

    return true;
}

// Read temperature data
bool MLX90392::readTemperature(int16_t& temperature) {
    uint8_t rawData[2];
    if (!readBytes(0x08, rawData, 2)) {
        return false;
    }

    // Convert raw data to 16-bit value
    temperature = (rawData[1] << 8) | rawData[0];

    return true;
}

// Read a specific register from the sensor
bool MLX90392::readRegister(uint8_t registerAddress, uint8_t& data) {
    uint8_t buffer[1];
    if (readBytes(registerAddress, buffer, 1)) {
        data = buffer[0];
        return true;
    }
    return false;
}

// Write to a specific register of the sensor
bool MLX90392::writeRegister(uint8_t registerAddress, uint8_t data) {
    uint8_t buffer[1] = { data };
    return writeBytes(registerAddress, buffer, 1);
}

// Set the application mode
bool MLX90392::setMode(Mode mode) {
    if (mode > 15) {
        return false; // Invalid mode
    }
    return writeRegister(0x10, static_cast<uint8_t>(mode));
}

// Get the current sensor mode
bool MLX90392::getMode(Mode& mode) {
    uint8_t ctrl;
    if (!readRegister(0x10, ctrl)) {
        return false;
    }
    mode = static_cast<Mode>(ctrl & 0x0F); // Extract MODE[3:0] bits
    return true;
}

// Read status register 1
bool MLX90392::readStatus1(uint8_t& status) {
    return readRegister(0x00, status);
}

// Read status register 2
bool MLX90392::readStatus2(uint8_t& status) {
    return readRegister(0x07, status);
}

// Check if data is ready
bool MLX90392::isDataReady() {
    uint8_t status;
    if (readStatus1(status)) {
        return status & 0x01; // Check DRDY bit (bit 0)
    }
    return false;
}

// Reset device
bool MLX90392::deviceReset() {
    if (writeRegister(0x11, 0x06)) {
        return true;
    }
    return false;
}

// Check if device was reset
bool MLX90392::isDeviceReset() {
    uint8_t status;
    if (readStatus1(status)) {
        return status & 0x08; // Check RT bit (bit 3)
    }
    return false;
}

// Check if data overrun occurred
bool MLX90392::isDataOverrun() {
    uint8_t status;
    if (readStatus2(status)) {
        return status & 0x02; // Check DOR bit (bit 1)
    }
    return false;
}

// Check if magnetic sensor overflow occurred
bool MLX90392::isMagneticSensorOverflow() {
    uint8_t status;
    if (readStatus2(status)) {
        return status & 0x01; // Check HOVF bit (bit 0)
    }
    return false;
}

// Perform a single measurement
bool MLX90392::singleMeasurement(int16_t data[3]) {
    // Set to single measurement mode
    if (!setMode(SINGLE_MEASUREMENT)) {
        return false;
    }

    // Wait until data is ready
    while (!isDataReady()) {
        delay(10); // Delay for a short period to avoid busy-waiting
    }

    // Read magnetic field data
    return readMagneticField(data);
}

// Perform a single measurement and return magnetic field data in microteslas (uT)
bool MLX90392::singleMeasurement_uT(float data[3]) {
    int16_t rawData[3];
    if (!singleMeasurement(rawData)) {
        return false;
    }

    float sensitivity = (sensorRange == RANGE_5MT) ? 0.15f : 1.5f;

    data[0] = rawData[0] * sensitivity;
    data[1] = rawData[1] * sensitivity;
    data[2] = rawData[2] * sensitivity;

    return true;
}

// Get the manufacturer ID
bool MLX90392::getManufacturerID(uint8_t& manufacturerID) {
    return readRegister(0x0A, manufacturerID);
}

// Get the device ID
bool MLX90392::getDeviceID(uint8_t& deviceID) {
    return readRegister(0x0B, deviceID);
}

// Set the oversampling ratio for magnetic measurements
bool MLX90392::setOversamplingRatio(bool ratio) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    osr = (osr & 0x7F) | (ratio << 7); // Set bit 7
    return writeRegister(0x14, osr);
}

// Get the oversampling ratio for magnetic measurements
bool MLX90392::getOversamplingRatio(bool& ratio) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    ratio = (osr & 0x80) >> 7; // Extract bit 7
    return true;
}

// Set the oversampling ratio for temperature measurements
bool MLX90392::setTemperatureOversamplingRatio(bool ratio) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    osr = (osr & 0xBF) | (ratio << 6); // Set bit 6
    return writeRegister(0x14, osr);
}

// Get the oversampling ratio for temperature measurements
bool MLX90392::getTemperatureOversamplingRatio(bool& ratio) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    ratio = (osr & 0x40) >> 6; // Extract bit 6
    return true;
}

// Set the digital filter for X and Y magnetic measurements
bool MLX90392::setXYDigitalFilter(uint8_t filter) {
    if (filter > 7) {
        return false; // Invalid filter value
    }
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    osr = (osr & 0xC7) | (filter << 3); // Set bits 5-3
    return writeRegister(0x14, osr);
}

// Get the digital filter for X and Y magnetic measurements
bool MLX90392::getXYDigitalFilter(uint8_t& filter) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    filter = (osr & 0x38) >> 3; // Extract bits 5-3
    return true;
}

// Set the digital filter for Z magnetic measurements
bool MLX90392::setZDigitalFilter(uint8_t filter) {
    if (filter > 7) {
        return false; // Invalid filter value
    }
    uint8_t custCtrl;
    if (!readRegister(0x15, custCtrl)) {
        return false;
    }
    custCtrl = (custCtrl & 0xF8) | (filter & 0x07); // Set bits 2-0
    custCtrl = (custCtrl & 0x7F) | (1 << 7); // Ensure bit 7 is set to 1
    custCtrl = (custCtrl & 0xBF); // Ensure bit 6 is set to 0
    custCtrl = (custCtrl & 0xEF); // Ensure bit 4 is set to 0
    custCtrl = (custCtrl & 0xF7); // Ensure bit 3 is set to 0
    return writeRegister(0x15, custCtrl);
}

// Get the digital filter for Z magnetic measurements
bool MLX90392::getZDigitalFilter(uint8_t& filter) {
    uint8_t custCtrl;
    if (!readRegister(0x15, custCtrl)) {
        return false;
    }
    filter = custCtrl & 0x07; // Extract bits 2-0
    return true;
}

// Set the digital filter for temperature measurements
bool MLX90392::setTemperatureDigitalFilter(uint8_t filter) {
    if (filter > 7) {
        return false; // Invalid filter value
    }
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    osr = (osr & 0xF8) | (filter & 0x07); // Set bits 2-0
    return writeRegister(0x14, osr);
}

// Get the digital filter for temperature measurements
bool MLX90392::getTemperatureDigitalFilter(uint8_t& filter) {
    uint8_t osr;
    if (!readRegister(0x14, osr)) {
        return false;
    }
    filter = osr & 0x07; // Extract bits 2-0
    return true;
}

// Enable or disable temperature measurement and compensation
bool MLX90392::setTemperatureCompensation(bool enable) {
    uint8_t custCtrl;
    if (!readRegister(0x15, custCtrl)) {
        return false;
    }
    custCtrl = (custCtrl & 0xDF) | (enable << 5); // Set bit 5
    custCtrl = (custCtrl & 0x7F) | (1 << 7); // Ensure bit 7 is set to 1
    custCtrl = (custCtrl & 0xBF); // Ensure bit 6 is set to 0
    custCtrl = (custCtrl & 0xEF); // Ensure bit 4 is set to 0
    custCtrl = (custCtrl & 0xF7); // Ensure bit 3 is set to 0
    return writeRegister(0x15, custCtrl);
}

// Get the state of temperature measurement and compensation
bool MLX90392::getTemperatureCompensation(bool& enable) {
    uint8_t custCtrl;
    if (!readRegister(0x15, custCtrl)) {
        return false;
    }
    enable = (custCtrl & 0x20) >> 5; // Extract bit 5
    return true;
}

// Set the sensor range
void MLX90392::setSensorRange(SensorRange range) {
    sensorRange = range;
}

// Get the sensor range
MLX90392::SensorRange MLX90392::getSensorRange() const {
    return sensorRange;
}

// Get magnetic field data in microteslas (uT)
bool MLX90392::getMagneticField_uT(float data[3]) {
    int16_t rawData[3];
    if (!readMagneticField(rawData)) {
        return false;
    }

    float sensitivity = (sensorRange == RANGE_5MT) ? 0.15f : 1.5f;

    data[0] = rawData[0] * sensitivity;
    data[1] = rawData[1] * sensitivity;
    data[2] = rawData[2] * sensitivity;

    return true;
}

// Perform a self-test and return the difference in magnetic field data
bool MLX90392::selfTest(int16_t data[3]) {
    Mode originalMode;
    if (!getMode(originalMode)) {
        return false;
    }

    // Switch to power down mode
    if (!setMode(POWER_DOWN)) {
        return false;
    }
    delay(100);

    // Switch to self-test mode
    if (!setMode(SELF_TEST)) {
        return false;
    }

    // Wait until data is ready
    while (!isDataReady()) {
        delay(10);
    }

    // Read magnetic field data after self-test
    int16_t selfTestData[3];
    if (!readMagneticField(selfTestData)) {
        return false;
    }

    delay(100);

    // Switch to single measurement mode
    if (!setMode(SINGLE_MEASUREMENT)) {
        return false;
    }

    // Wait until data is ready
    while (!isDataReady()) {
        delay(10);
    }

    // Read magnetic field data after single measurement
    int16_t singleMeasurementData[3];
    if (!readMagneticField(singleMeasurementData)) {
        return false;
    }

    // Restore the original mode
    if (!setMode(originalMode)) {
        return false;
    }

    // Calculate the difference between the two measurements
    for (int i = 0; i < 3; i++) {
        data[i] = singleMeasurementData[i] - selfTestData[i];
    }

    return true;
}

// Helper function to read multiple bytes
bool MLX90392::readBytes(uint8_t registerAddress, uint8_t* data, size_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    if (Wire.endTransmission(false) != 0) { // Send repeated START condition
        return false;
    }

    Wire.requestFrom(deviceAddress, length);
    for (size_t i = 0; i < length; i++) {
        if (Wire.available()) {
            data[i] = Wire.read();
        }
        else {
            return false;
        }
    }
    return true;
}

// Helper function to write multiple bytes
bool MLX90392::writeBytes(uint8_t registerAddress, const uint8_t* data, size_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(registerAddress);
    for (size_t i = 0; i < length; i++) {
        Wire.write(data[i]);
    }
    return Wire.endTransmission() == 0;
}
