#include <esp_now.h>
#include <WiFi.h>

// Adresse MAC de l'émetteur
uint8_t senderAddress[] = {0x08, 0x3A, 0xF2, 0xAB, 0xB0, 0x0C};
// Structure pour les états des boutons
struct ButtonStates {
    int leftButtonState;
    int rightButtonState;
};

ButtonStates *incomingData; // Déclarer comme pointeur

// Fonction appelée lors de la réception des données
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingDataBytes, int len) {
    incomingData = (ButtonStates*)incomingDataBytes; // Convertir en pointeur ButtonStates
    Serial.print("État du bouton gauche : ");
    Serial.println(incomingData->leftButtonState);
    Serial.print("État du bouton droit : ");
    Serial.println(incomingData->rightButtonState);
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Démarrage du récepteur ESP-NOW");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0) {
        Serial.println("Erreur d'initialisation ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    // Rien à faire ici
}
