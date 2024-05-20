#include <esp_now.h>
#include <WiFi.h>

#define PIN_BTN_GAUCHE 13
#define PIN_BTN_DROITE 12

// Remplacez par les adresses MAC du récepteur que vous avez obtenues
uint8_t receiverAddress[] = {0x24, 0x6F, 0x28, 0x7B, 0x7C, 0xF0};

// Structure pour les états des boutons
struct ButtonStates {
    int leftButtonState;
    int rightButtonState;
};

ButtonStates buttonStates;

// Fonction appelée lors de l'envoi des données
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Statut d'envoi : ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Démarrage de l'émetteur ESP-NOW");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0) {
        Serial.println("Erreur d'initialisation ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Échec de l'ajout du pair");
        return;
    }

    pinMode(PIN_BTN_GAUCHE, INPUT);
    pinMode(PIN_BTN_DROITE, INPUT);
}

void loop() {
    buttonStates.leftButtonState = digitalRead(PIN_BTN_GAUCHE);
    buttonStates.rightButtonState = digitalRead(PIN_BTN_DROITE);

    // Envoi des données
    uint8_t result = esp_now_send(receiverAddress, (uint8_t *) &buttonStates, sizeof(buttonStates));

    if (result == ESP_OK) {
        Serial.println("Envoi réussi");
    } else {
        Serial.println("Erreur d'envoi");
    }
    
    delay(1000);
}
