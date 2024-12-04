#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

// I2C Slave Address
#define I2C_ADDRESS 8

// Default velocity for Note On messages
#define DEFAULT_VELOCITY 100

// Number of keys on the keyboard
#define NUM_NOTES 54

// Pins for each key (digital inputs)
const uint8_t notePins[NUM_NOTES] = {
    2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
    46, 47, 48, 49, 50, 51, 52, 53, A0, A1, A2, A3, A4, A5
};

// Corresponding MIDI note numbers
const uint8_t noteMidiNumbers[NUM_NOTES] = {
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85
};

#endif
