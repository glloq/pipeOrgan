#include <Wire.h>
#include "MegaSettings.h"

// Constants
#define MIDI_QUEUE_SIZE 32 // Size of the FIFO queue for MIDI messages

// Structure for MIDI messages
struct MidiMessage {
    uint8_t status; // Status byte (e.g., 0x90 for Note On, 0x80 for Note Off)=> for channel 2 it's 0x91:0x82 => channel 8 it's 0x98;0x88
    uint8_t data1;  // Note number or control number
    uint8_t data2;  // Velocity or value
};

// FIFO Queue for MIDI messages
MidiMessage midiQueue[MIDI_QUEUE_SIZE];
volatile uint8_t queueHead = 0;
volatile uint8_t queueTail = 0;

// Function to add a message to the FIFO
bool enqueueMidiMessage(uint8_t status, uint8_t data1, uint8_t data2) {
    uint8_t nextHead = (queueHead + 1) % MIDI_QUEUE_SIZE;
    if (nextHead == queueTail) {
        // Queue is full, message is dropped
        return false;
    }
    midiQueue[queueHead] = {status, data1, data2};
    queueHead = nextHead;
    return true;
}

// Function to retrieve a message from the FIFO
bool dequeueMidiMessage(MidiMessage &msg) {
    if (queueHead == queueTail) {
        // Queue is empty
        return false;
    }
    msg = midiQueue[queueTail];
    queueTail = (queueTail + 1) % MIDI_QUEUE_SIZE;
    return true;
}

// Keyboard configuration
bool keyStates[NUM_NOTES]; // Current state of each key

void setup() {
    Wire.begin(I2C_ADDRESS); // Initialize I2C as slave
    for (uint8_t i = 0; i < NUM_NOTES; i++) {
        pinMode(notePins[i], INPUT_PULLUP); // Set all key pins to INPUT with pull-up
        keyStates[i] = false; // Initialize all keys as "not pressed"
    }
}

void loop() {
    // Scan the keyboard
    for (uint8_t i = 0; i < NUM_NOTES; i++) {
        bool currentState = !digitalRead(notePins[i]); // Active LOW
        if (currentState != keyStates[i]) {
            keyStates[i] = currentState;
            uint8_t status = currentState ? 0x90 : 0x80; // Note On or Note Off
            enqueueMidiMessage(status, noteMidiNumbers[i], currentState ? DEFAULT_VELOCITY : 0);
        }
    }

    // Send messages from the queue via I2C
    sendMidiMessages();
}

void sendMidiMessages() {
    MidiMessage msg;
    while (dequeueMidiMessage(msg)) {
        Wire.beginTransmission(I2C_ADDRESS);
        Wire.write(msg.status);
        Wire.write(msg.data1);
        Wire.write(msg.data2);
        Wire.endTransmission();
    }
}
