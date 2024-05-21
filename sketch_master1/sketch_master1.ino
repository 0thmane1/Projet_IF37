#include <esp_now.h>
#include <WiFi.h>

#define SERIAL_BAUD 115200
#define PIN_BTN_GAUCHE 13
#define PIN_BTN_DROITE 12

uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x7B, 0x7C, 0xF0}; // Remplacer par l'adresse MAC du récepteur

// Structure pour envoyer les données
typedef struct struct_message {
    bool ledVert;   // Booléen pour représenter l'état de la LED verte
    bool ledRouge;  // Booléen pour représenter l'état de la LED rouge
} struct_message;

struct_message myData;

// Callback lorsque les données sont envoyées
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nMaster packet sent: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
    // Initialisation du moniteur série
    Serial.begin(SERIAL_BAUD);

    // Initialiser les boutons comme entrée
    pinMode(PIN_BTN_GAUCHE, INPUT);
    pinMode(PIN_BTN_DROITE, INPUT);

    // Configurer le dispositif comme station Wi-Fi
    WiFi.mode(WIFI_STA);

    // Initialiser ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    Serial.print("Transmitter initialized: ");
    Serial.println(WiFi.macAddress());

    // Définir la fonction d'envoi
    esp_now_register_send_cb(OnDataSent);

    // Enregistrer le peer
    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // Ajouter le peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    // Lire l'état des boutons
    myData.ledVert = digitalRead(PIN_BTN_GAUCHE) == HIGH;
    myData.ledRouge = digitalRead(PIN_BTN_DROITE) == HIGH;

    // Envoyer l'état des LEDs
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
        Serial.println("Sent with success");
    } else {
        Serial.println("Error sending the data");
    }

    delay(10);  // Petit délai pour stabiliser les lectures du bouton
}
