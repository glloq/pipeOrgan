#include <Wire.h>
#include <MIDIUSB.h>

// I2C Slave Addresses for the Mega devices
#define MEGA1_ADDRESS 8 // keyboard 1 
#define MEGA2_ADDRESS 9 // keyboard 2
#define MEGA3_ADDRESS 10 // keyboard pedals and other?
#define MEGA3_ADDRESS 11 // other register, pistons expression pedal etc.. ?

void setup() {
    // Initialize I2C as master
    Wire.begin();
    Serial.begin(115200);
}

void loop() {
    // Check for MIDI messages from each Mega
    checkMidiMessages(MEGA1_ADDRESS);
    checkMidiMessages(MEGA2_ADDRESS);
    checkMidiMessages(MEGA3_ADDRESS);
    checkMidiMessages(MEGA4_ADDRESS);
    delay(10);  // Small delay for stability
}

void checkMidiMessages(uint8_t slaveAddress) {
    Wire.beginTransmission(slaveAddress);
    Wire.write(0x00);  // Command to request MIDI message from the slave
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 3); // Request 3 bytes (MIDI message)
    if (Wire.available() == 3) {
        uint8_t status = Wire.read();
        uint8_t data1 = Wire.read();
        uint8_t data2 = Wire.read();
        
        // Send the MIDI message to the raspberry via USB
        MidiUSB.sendMIDI(status, data1, data2);
        MidiUSB.flush();
    }
}
