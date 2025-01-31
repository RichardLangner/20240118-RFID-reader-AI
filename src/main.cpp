#include <Arduino.h>
#include <LittleFS.h>

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

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount the filesystem.");
        return;
    }

    // Create a test file
    // createTestFile(); I comment out then upload from the data folder, and it overwrites the test file

    // List the files after creating the test file
    printLittleFSContents();
}

void loop() {
    // Empty loop, just run once in setup
}
