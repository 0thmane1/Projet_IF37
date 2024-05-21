#include <esp_now.h>
#include <WiFi.h>

#define SERIAL_BAUD 115200
#define PIN_VERT_GAUCHE 26
#define PIN_ROUGE_DROITE 5

// Structure pour recevoir les données
typedef struct struct_message {
    bool ledVert;   // Booléen pour représenter l'état de la LED verte
    bool ledRouge;  // Booléen pour représenter l'état de la LED rouge
} struct_message;

struct_message myData;

// Callback lorsque les données sont reçues
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    Serial.println("Data received callback");
    memcpy(&myData, incomingData, sizeof(myData));

    // Contrôler la LED verte en fonction des données reçues
    if (myData.ledVert) {
        digitalWrite(PIN_VERT_GAUCHE, HIGH); // Allumer la LED verte
    } else {
        digitalWrite(PIN_VERT_GAUCHE, LOW); // Éteindre la LED verte
    }

    // Contrôler la LED rouge en fonction des données reçues
    if (myData.ledRouge) {
        digitalWrite(PIN_ROUGE_DROITE, HIGH); // Allumer la LED rouge
    } else {
        digitalWrite(PIN_ROUGE_DROITE, LOW); // Éteindre la LED rouge
    }

    Serial.print("LED Vert State: ");
    Serial.println(myData.ledVert);
    Serial.print("LED Rouge State: ");
    Serial.println(myData.ledRouge);
}

void setup() {
    // Initialisation du moniteur série
    Serial.begin(SERIAL_BAUD);

    // Initialiser les LEDs comme sortie
    pinMode(PIN_VERT_GAUCHE, OUTPUT);
    pinMode(PIN_ROUGE_DROITE, OUTPUT);
    digitalWrite(PIN_VERT_GAUCHE, LOW); // S'assurer que la LED verte est éteinte au départ
    digitalWrite(PIN_ROUGE_DROITE, LOW); // S'assurer que la LED rouge est éteinte au départ

    // Configurer le dispositif comme station Wi-Fi
    WiFi.mode(WIFI_STA);

    // Imprimer l'adresse MAC du récepteur
    Serial.print("Slave MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Initialiser ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Enregistrer la fonction de réception
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Rien à faire ici
}
