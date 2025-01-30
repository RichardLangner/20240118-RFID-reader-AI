#include <Arduino.h>  // Include the Arduino library for PlatformIO
#include <LittleFS.h>  // Include the LittleFS library
#include <SoftwareSerial.h>  // Include the SoftwareSerial library

#define RX_PIN D2  // Change RX_PIN to D2
#define TX_PIN D6  // Define TX_PIN for SoftwareSerial
#define BUILTIN_LED LED_BUILTIN  // Define built-in LED for ESP8266

SoftwareSerial rfidSerial(RX_PIN, TX_PIN);  // Create a SoftwareSerial instance

// Declare the writeToFile function at the beginning to avoid compiler issues
void writeToFile(const char* data);
void clearFile(const char* path);  // Declare a function to clear the file
String rfidData = "";

void setup() {
    // Initialize Serial Monitor
    Serial.begin(9600);  // Change the baud rate to 9600

    // Initialize SoftwareSerial
    rfidSerial.begin(9600);
    Serial.println("RFID Reader Ready.");

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount the file system");
        return;
    }

    // Initialize built-in LED pin as OUTPUT
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);  // Ensure the LED is off initially (HIGH for active low LED)

    // Announce that the RFID scanning works
    Serial.println("Place your RFID tag near the reader...");

    // Clear the stored file contents
    clearFile("/rfidData.txt");
    writeToFile("00E6021F982846");
   // Print the contents of the stored file for comparison
    Serial.println("Contents of the stored file:");
    File file = LittleFS.open("/rfidData.txt", "r");
    if (file) {
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
    } else {
        Serial.println("Failed to open the file for reading.");
    }
    Serial.println();
}

void loop() {
    // Check for RFID data periodically
    // static unsigned long lastCheckTime = 0;
    // const unsigned long checkInterval = 1000;  // Check every 1 second
    // if (millis() - lastCheckTime > checkInterval) {
    //     lastCheckTime = millis();
    //     Serial.println("Checking for RFID data...");

    // Check if data is available from the RFID reader
    if (rfidSerial.available() > 0) {
        Serial.println("Data available from RFID reader.");
        while (rfidSerial.available() > 0) {
            char c = rfidSerial.read();
            rfidData += c;
        }
        if(rfidData.length()>15){
        rfidData=rfidData.substring(1, rfidData.length()-2);
        Serial.print("Raw RFID Data: ");
        Serial.println(rfidData);
        rfidData="";
        }
    }

    //         // Compare the raw data directly with the known tag UID
    //         // String knownTag = rfidData;  // Use current RFID data as knownTag

    //         // if (rfidData.equals(knownTag)) {
    //             // Serial.println("Tag match found - Turning on LED");
    //             // digitalWrite(BUILTIN_LED, LOW);  // Turn on the built-in LED (LOW for active low LED)
    //         // } else {
    //             // Serial.println("No match found");
    //             // digitalWrite(BUILTIN_LED, HIGH);  // Turn off the built-in LED
    //         // }

    //         // Write data to a file
    //         // writeToFile(rfidData.c_str());
    //     } else {
    //         Serial.println("No data available from RFID reader.");
    //         digitalWrite(BUILTIN_LED, HIGH);  // Ensure the LED stays off when no data is available
    //     }
    // }
}

// Function to clear the contents of a file
void clearFile(const char* path) {
    File file = LittleFS.open(path, "w");
    if (file) {
        file.close();
        Serial.println("File contents cleared.");
    } else {
        Serial.println("Failed to clear the file.");
    }
}

// Definition of the writeToFile function
void writeToFile(const char* data) {
    // Open the file for writing
    File file = LittleFS.open("/rfidData.txt", "a");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    // Write data to the file
    if (file.print(data)) {
        Serial.println("Data successfully written to the file");
    } else {
        Serial.println("Write failed");
    }

    // Close the file
    file.close();
}
