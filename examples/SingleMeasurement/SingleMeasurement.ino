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
}

void loop() {
    float magneticField[3];

    // Perform a single measurement and get the magnetic field data in uT
    if (sensor.singleMeasurement_uT(magneticField)) {
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

