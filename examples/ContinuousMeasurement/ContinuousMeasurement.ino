#include <Wire.h>
#include "mlx90392_lib.h"

// Create an instance of the MLX90392 class
MLX90392 sensor;

void setup() {
    // Initialize Serial communication
    Serial.begin(9600);
    while (!Serial) {
        ; // Wait for Serial to be ready
    }

    // Initialize the sensor
    if (!sensor.init()) {
        Serial.println("Failed to initialize the sensor!");
        while (1);
    }

    // Set the sensor range to +/- 5 mT
    sensor.setSensorRange(MLX90392::RANGE_5MT);

    // Set the oversampling rate to 1
    if (!sensor.setOversamplingRatio(true)) {
        Serial.println("Failed to set oversampling ratio.");
    }

    // Set the digital filter for X and Y to 3
    if (!sensor.setXYDigitalFilter(3)) {
        Serial.println("Failed to set XY digital filter.");
    }

    // Set the digital filter for Z to 4
    if (!sensor.setZDigitalFilter(4)) {
        Serial.println("Failed to set Z digital filter.");
    }

    // Print a message indicating that the sensor is ready
    Serial.println("Sensor initialized and ready.");

    // Get and print the manufacturer ID
    uint8_t manufacturerID;
    if (sensor.getManufacturerID(manufacturerID)) {
        Serial.print("Manufacturer ID: 0x");
        Serial.println(manufacturerID, HEX);
    } else {
        Serial.println("Failed to read Manufacturer ID.");
    }

    // Get and print the device ID
    uint8_t deviceID;
    if (sensor.getDeviceID(deviceID)) {
        Serial.print("Device ID: 0x");
        Serial.println(deviceID, HEX);
    } else {
        Serial.println("Failed to read Device ID.");
    }

    // Set the sensor to continuous measurement mode at 10Hz
    if (!sensor.setMode(MLX90392::CONTINUOUS_10HZ)) {
        Serial.println("Failed to set continuous measurement mode.");
    }
}

void loop() {
    float magneticField[3];

    // Perform a single measurement and get the magnetic field data in uT
    if (sensor.getMagneticField_uT(magneticField)) {
        // Print the magnetic field data
        Serial.print("Magnetic Field (uT) - X: ");
        Serial.print(magneticField[0]);
        Serial.print(", Y: ");
        Serial.print(magneticField[1]);
        Serial.print(", Z: ");
        Serial.println(magneticField[2]);
    } else {
        Serial.println("Failed to perform measurement.");
    }

    // Wait for 1 second before the next measurement
    delay(1000);
}


