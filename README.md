> [!NOTE]
> juste un regroupement d'idées pour l'instant

# pipeOrgan
l'objectif est de comprendre comment fabriquer facilement un systeme midi qui controle un orgue depuis 3 clavier et avec differents registres.

## l'utilisation de midi 

sur un systeme de clavier d'orgue, chaque clavier est associé a un canal midi.

la gestion des differents registres est faite via l'utilisation de messages control change (CC) ou program change (PC)

l'activation de tuyaux en plus est faite par le controleur du coté tuyaux, il suffit d'envoyer un message CC ou PC sur un canal midi pour indiquer qu'il faut ouvrir certain type de tuyaux (c'est a adapter en fonction de l'orgue! )

## choix electroniques

Pour simplifier le code et les couts, j'ai choisis d'utiliser des mcp23017 en matrice pour chaque clavier afin de permettre jusqu'a 64 entrées par clavier/chanel midi.
il faut pensser a utiliser des diodes sur chaque entré afin pour éviter le phénomène de "ghosting" (fantômes) et de "key rollover" (erreurs lors de la détection de plusieurs touches simultanées).
<img src="https://raw.githubusercontent.com/glloq/pipeOrgan/main/matrice%208x8png" alt="matrice 8x8" width=80% height=80%/>
  
le mcp permet l'utilisation d'interuption avec les pin INTA et INTB, pour une meilleure reactivité.
INTA et INTB : Ces deux broches d'interruption peuvent être associées indépendamment aux broches du port A (A0-A7) et du port B (B0-B7) respectivement.
- INTA est généralement liée aux broches du port A.
- INTB est liée aux broches du port B.

Port A (Colonnes) : Les broches A0-A7 sont configurées comme sorties. Nous activons successivement chaque colonne pour détecter quel bouton est appuyé.

Port B (Lignes) : Les broches B0-B7 sont configurées comme entrées avec résistances pull-up. Ces broches génèrent une interruption lorsque l'état de l'une d'elles change (lorsqu'un bouton est pressé).

## le code 

exemple pour la lecture d'un mcp23017 en matrice 8x8.

```
#include <Wire.h>
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;

class ButtonMatrix {
  public:
    void setup() {
      mcp.begin();  // Initialiser le MCP23017 avec l'adresse par défaut 0x20

      // Configurer les colonnes (A0-A7) comme sorties et les lignes (B0-B7) comme entrées
      for (int i = 0; i < 8; i++) {
        mcp.pinMode(i, OUTPUT);        // Colonnes
        mcp.digitalWrite(i, HIGH);     // Colonnes à HIGH par défaut (désactivées)

        mcp.pinMode(i + 8, INPUT);     // Lignes
        mcp.pullUp(i + 8, HIGH);       // Activer les résistances pull-up sur les lignes
      }

      // Configurer les interruptions sur les lignes (B0-B7)
      mcp.setupInterrupts(true, false, LOW);  // Activer les interruptions sur INTA, active bas

      for (int i = 8; i < 16; i++) {
        mcp.setupInterruptPin(i, FALLING); // Déclencher l'interruption sur front descendant
      }

      // Configurer la broche d'interruption sur le microcontrôleur
      pinMode(2, INPUT);  // INTA connectée à la pin 2 d'Arduino
      attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, FALLING);  // Gérer l'interruption
    }

    void scanMatrix() {
      for (int col = 0; col < 8; col++) {
        // Activer la colonne actuelle
        mcp.digitalWrite(col, LOW);

        // Lire les lignes (B0-B7)
        for (int row = 0; row < 8; row++) {
          if (mcp.digitalRead(row + 8) == LOW) { // Si la ligne est à LOW, un bouton est pressé
            Serial.print("Button pressed at Row: ");
            Serial.print(row);
            Serial.print(", Column: ");
            Serial.println(col);
            // on pourra envoyer le message de la note midi correspondante (FIRST MIDI NOTE + row x 8 + col) au canal du clavier utilisé
          }
        }

        // Désactiver la colonne actuelle
        mcp.digitalWrite(col, HIGH);
      }
    }

    // Gestion de l'interruption
    static void handleInterrupt() {
      // Gérer l'interruption : détecter quelle broche a déclenché l'interruption
      uint8_t intPin = mcp.getLastInterruptPin();
      uint8_t intVal = mcp.getLastInterruptPinValue();

      if (intPin >= 8 && intVal == LOW) {
        // Si une ligne a généré une interruption, scanner la matrice
        ButtonMatrix::instance.scanMatrix();
      }
    }

    // Singleton pour accéder à l'instance depuis l'interruption
    static ButtonMatrix instance;
};

ButtonMatrix ButtonMatrix::instance;

void setup() {
  Serial.begin(9600);
  ButtonMatrix::instance.setup();  // Configurer la matrice de boutons
}

void loop() {
  // les boutons sont gérés via interruptions
}

```

### les claviers

il faut indiquer l'adresse du mcp, le numero midi de la note la plus grave et le nombre de note du calvier.
les touches du clavier seront incrementé dans l'ordre ligne par ligne.


### les registres 

les registres seront géré avec un tablau [8,8] e structure : 

```
struct Registre {
    String nom;                 // Nom du registre
    uint8_t numeroCC;           // Numéro de Control Change MIDI => ou PC en fonction de l'ogue
    uint8_t canalMIDI;          // Canal MIDI
    bool etatActuel;            // Etat du registre (ON/OFF)
    uint8_t adresseMCP;         // Adresse I2C du MCP23017
    uint8_t colPinMCP;          // Numéro de pin colonne MCP23017
    uint8_t rowPinMCP;          // Numéro de pin ligne MCP23017
};
```
la lecture des registres est faite de la meme maniere que pour les clavier, lors d'une modification d'une valeur, il faut envoyer le message cc ou pc adapté.

### la pedales

il y a une pedales à gerer:
- La pédale d'expression permet de moduler dynamiquement le volume de certaines divisions de l'orgue.

on viendra lire la valeurs d'un potentiometre et on revoit un message control change (7 ou 11) d'une valeur de 0 a 127.


