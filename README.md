> [!NOTE]
> juste un regroupement d'idées pour l'instant

# pipeOrgan
l'objectif est de comprendre comment fabriquer facilement un systeme midi qui controle un orgue depuis 3 clavier et avec differents registres.

## l'utilisation de midi 

sur un systeme de clavier d'orgue, chaque clavier est associé a un canal midi.
la gestion des differents registres est faite via l'utilisation de messages control change (CC).

## choix electroniques

pour simplifier le code et les couts, j'ai choisis d'utiliser des mcp23017 en matrice pour chaque clavier afin de permettre jusqu'a 64 entrées par clavier.
il faut pensser a utiliser des diodes sur chaque entré afin de permettre une lecture des entrées plus stable.

le mcp permet l'utilisation d'interuption avec les pin INTA et INTB, pour une meilleure reactivité

## le code 

### les claviers

il faut indiquer l'adresse du mcp, le numero midi de la note la plus grave et le nombre de note du calvier.
les touches du clavier seront incrementé dans l'ordre ligne par ligne.

### les registres 

les registres seront fait avec un tablau de structure : 

```
struct Registre {
    String nom;                 // Nom du registre
    uint8_t numeroCC;           // Numéro de Control Change MIDI
    uint8_t canalMIDI;          // Canal MIDI
    bool etatActuel;            // Etat du registre (ON/OFF)
    uint8_t adresseMCP;         // Adresse I2C du MCP23017
    uint8_t numeroPinMCP;       // Numéro de pin sur le MCP23017
};
```
