#include <Wire.h>
#include "MegaSettings.h"

#define FIFO_SIZE 32

// Structure pour stocker un message MIDI
struct MidiMessage {
  byte status;
  byte data1;
  byte data2;
};

// Buffer FIFO
MidiMessage fifo[FIFO_SIZE];
volatile int fifoHead = 0;
volatile int fifoTail = 0;

// Fonction pour ajouter un message au buffer FIFO
bool enqueueMessage(MidiMessage message) {
  int nextHead = (fifoHead + 1) % FIFO_SIZE;
  if (nextHead == fifoTail) {
    // Buffer plein, le message est ignoré
    return false;
  }
  fifo[fifoHead] = message;
  fifoHead = nextHead;
  return true;
}

// Fonction pour retirer un message du buffer FIFO
bool dequeueMessage(MidiMessage &message) {
  if (fifoHead == fifoTail) {
    // Buffer vide
    return false;
  }
  message = fifo[fifoTail];
  fifoTail = (fifoTail + 1) % FIFO_SIZE;
  return true;
}

// Tableau pour stocker l'état précédent des touches
bool lastState[NUM_NOTES] = {0};

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendNextMidiMessage); // Attache la fonction de réponse
  for (int i = 0; i < NUM_NOTES; i++) {
    pinMode(midiInput[i].pin, INPUT_PULLUP);
  }
}

void loop() {
  // Détecter les changements d'état et ajouter les messages au buffer
  for (int i = 0; i < NUM_NOTES; i++) {
    bool currentState = digitalRead(midiInput[i].pin);

    if (currentState != lastState[i]) {
      lastState[i] = currentState; // Mettre à jour l'état précédent

      MidiMessage message;
      if (currentState == HIGH) { // Touche pressée
        if (midiInput[i].messageType == 0) { // Note
          message.status = 0x90 + midiInput[i].midiChannel;
          message.data1 = midiInput[i].param1;
          message.data2 = 127; // Vélocité max
        } else if (midiInput[i].messageType == 1) { // Program Change
          message.status = 0xC0 + midiInput[i].midiChannel;
          message.data1 = midiInput[i].param1;
          message.data2 = 0;
        }
      } else { // Touche relâchée
        if (midiInput[i].messageType == 0) {
          message.status = 0x80 + midiInput[i].midiChannel;
          message.data1 = midiInput[i].param1;
          message.data2 = 0;
        } else if (midiInput[i].messageType == 1) {
          message.status = 0xC0 + midiInput[i].midiChannel;
          message.data1 = midiInput[i].param2;
          message.data2 = 0;
        }
      }
      enqueueMessage(message); // Ajouter le message au buffer
    }
  }

  delay(DEBOUNCE_TIME); // Éviter les rebonds
}

// Fonction appelée par le maître pour récupérer un message MIDI
void sendNextMidiMessage() {
  MidiMessage message;
  if (dequeueMessage(message)) {
    Wire.write(message.status);
    Wire.write(message.data1);
    Wire.write(message.data2);
  } else {
    Wire.write(0); // Pas de message disponible
  }
  Wire.endTransmission();
}
