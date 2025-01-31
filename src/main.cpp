#include <Arduino.h>
#include <LittleFS.h>
#include <SoftwareSerial.h>  // Include the SoftwareSerial library
#include "BlinkDigits.h"

#define RX_PIN D2  // Change RX_PIN to D2
#define TX_PIN D6  // Define TX_PIN for SoftwareSerial
#define BUILTIN_LED LED_BUILTIN  // Define built-in LED for ESP8266
enum RFIDState {
    WAIT_FOR_STX, // Waiting for the start of the message (STX)
    READ_DATA,    // Reading the RFID data
    PROCESS_DATA  // Process the complete RFID data
};
RFIDState rfidState = WAIT_FOR_STX; // Initial state
String rfidData = "";
int tagNumber = 0;    // Number is stored in the file

SoftwareSerial rfidSerial(RX_PIN, TX_PIN);  // Create a SoftwareSerial instance
BlinkDigits flasher1; 

void printLittleFSContents() {
    Dir dir = LittleFS.openDir("/");

    Serial.println("Listing files in LittleFS:");

    while (dir.next()) {
        String fileName = dir.fileName();
        uint32_t fileSize = dir.fileSize();

        Serial.print("File: ");
        Serial.print(fileName);
        Serial.print(" | Size: ");
        Serial.println(fileSize);
    }
    Serial.println("Finished listing files.");
}

void createTestFile() {
    // Open a file in write mode. If the file doesn't exist, it will be created.
    File file = LittleFS.open("test_file.txt", "w");

    if (file) {
        Serial.println("Creating test file in LittleFS...");

        // Write some content to the file
        file.println("Hello, LittleFS!");
        file.close();  // Don't forget to close the file!

        Serial.println("Test file created.");
    } else {
        Serial.println("Failed to create test file.");
    }
}

void setup() {
    // Start serial communication
    Serial.begin(74880);

    // Initialize SoftwareSerial
    rfidSerial.begin(9600);
    Serial.println("RFID Reader Ready.");

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount the filesystem.");
        return;
    }

    // Create a test file
    // createTestFile(); I comment out then upload from the data folder, and it overwrites the test file

    // List the files after creating the test file
    printLittleFSContents();

    // Initialize built-in LED pin as OUTPUT
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);  // Ensure the LED is off initially (HIGH for active low LED)

    // Announce that the RFID scanning works
    Serial.println("Place your RFID tag near the reader...");
}

void loop() {
    // Flash LED according to tag number
    flasher1.blink(BUILTIN_LED, LOW, tagNumber);

    // Process one character at a time
    if (rfidSerial.available() > 0) {
        char c = rfidSerial.read();

        switch (rfidState) {
            case WAIT_FOR_STX:  // Wait for the STX character (0x02)
                if (c == 0x02) {
                    rfidState = READ_DATA; // Move to the next state
                    rfidData = "";        // Clear the buffer
                    Serial.println("Start of RFID data detected.");
                }
                break;

            case READ_DATA:  // Read data until the ETX character (0x03)
                if (c == 0x03) {
                    rfidState = PROCESS_DATA; // Move to the next state
                } else {
                    rfidData += c; // Append the character to the buffer
                }
                break;
        }
    }

    // Handle PROCESS_DATA state outside the serial available check
    if (rfidState == PROCESS_DATA) {    // Process the complete RFID data
        rfidData.trim();    // Trim non-printable characters (e.g., newline, carriage return)
        
        // Check if the data is 14 characters long
        if (rfidData.length() == 14) {
            Serial.print("Trimmed RFID Data: ");
            Serial.println(rfidData);

            // Check if a file with the RFID data name exists in LittleFS
            if (LittleFS.exists(rfidData)) {
                Serial.println("File exists in LittleFS.");
                // Read contents of the file. This determines the number of LED flashes.
                // Open the file for reading
            File file = LittleFS.open(rfidData, "r");
            if (!file) {
                Serial.println("Failed to open file for reading.");
            } else {
                // Read the file contents (number as text)
                String fileContent = file.readString();
                file.close(); // Close the file

                // Convert the file content to an integer
                tagNumber = fileContent.toInt();

                // Print the tag number
                Serial.print("Tag Number: ");
                Serial.println(tagNumber);

                // Use the tagNumber for further processing (e.g., LED flashes)
                // Example: flashLED(tagNumber);
            }
            } else {
                Serial.println("File does not exist in LittleFS.");
            }
        } else {
            Serial.println("Invalid RFID data length.");
            Serial.print("Length: ");
            Serial.println(rfidData.length());
        }

        // Reset the state machine
        rfidState = WAIT_FOR_STX;
        rfidData = ""; // Clear the buffer for the next read
    }

    // Add other non-blocking tasks here
    // For example: blink an LED, read sensors, etc.
}