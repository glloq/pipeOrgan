#ifndef SETTINGS_H
#define SETTINGS_H

// Nombre total d'inputs
#define NUM_NOTES 54

// Structure pour regrouper tous les paramètres pour chaque touche
typedef struct {
  byte midiChannel;     // Canal MIDI (0-15)
  int pin;              // Pin associée à la touche (entrée numérique)
  byte messageType;     // Type de message MIDI (0 pour Note, 1 pour Program Change)
  byte param1;          // Paramètre 1 (Numéro de la note MIDI ou valeur pour activer un PC)
  byte param2;          // Paramètre 2 (Utilisé pour désactiver un PC)
} MidiInput;


// put each inputs here 
MidiInput midiInput[NUM_NOTES] = {
  // Format : {channel, pin, messageType (note=0 or PC=1) , param1, param2}

  // Exemple  message Note for MIDI 60 (C4)
  { 0, 2, 0, 60, 0 },  // Channel 0, Pin 2, Note (MIDI 60 - C4), the last one is not used for a note

  // Exemple pour un message Program Change
  { 0, 3, 1, 0, 1 },   // Channel 0, Pin 3, Program Change (param1 = 0 midi ONn, param2 = 1 Midi OFF)=> bourdon 16' on your pipe organ
  { 0, 4, 1, 2, 3 },   // Channel 0, Pin 3, Program Change (param1 = 2 midi ONn, param2 = 3 Midi OFF)=> montre 8' on your pipe organ

  // Ajouter les autres notes avec les mêmes paramètres (canal, pin, messageType, param1, param2)
};


// Adresse de l'esclave I2C (Arduino Mega)
#define SLAVE_ADDRESS 8

// Intervalle de balayage du clavier (en ms)
#define DEBOUNCE_TIME 5

#endif // SETTINGS_H
