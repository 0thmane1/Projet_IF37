#include <esp_now.h>
#include <WiFi.h>

#define SERIAL_BAUD 115200
#define LED_PIN_ROUGE 26
#define LED_PIN_BLEU 27
#define LED_PIN_BLEU_2 25
#define VIBREUR_PIN 33

// Structure pour recevoir les données
typedef struct struct_message {
    bool alarm;  // Booléen pour représenter l'état de l'alarme
} struct_message;

struct_message myData;
bool alarmActive = false;
unsigned long previousMillis = 0;
const long interval = 500; // Intervalle de clignotement en millisecondes

// Callback lorsque les données sont reçues
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    Serial.println("Data received callback");
    memcpy(&myData, incomingData, sizeof(myData));

    if (myData.alarm) {
        // Activer le vibreur
        digitalWrite(VIBREUR_PIN, HIGH);
        alarmActive = true;
    } else {
        // Désactiver la LED et le vibreur
        digitalWrite(VIBREUR_PIN, LOW);
        eteindreLED();
        alarmActive = false;
    }
}

void allumerLEDRouge() {
    digitalWrite(LED_PIN_ROUGE, HIGH);
    digitalWrite(LED_PIN_BLEU, LOW);
    digitalWrite(LED_PIN_BLEU_2, LOW);
}

void allumerLEDBleu() {
    digitalWrite(LED_PIN_ROUGE, LOW);
    digitalWrite(LED_PIN_BLEU, HIGH);
    digitalWrite(LED_PIN_BLEU_2, HIGH);
}

void eteindreLED() {
    digitalWrite(LED_PIN_ROUGE, LOW);
    digitalWrite(LED_PIN_BLEU, LOW);
    digitalWrite(LED_PIN_BLEU_2, LOW);
}

void setup() {
    // Initialisation du moniteur série
    Serial.begin(SERIAL_BAUD);

    // Initialiser les broches de la LED et le vibreur comme sortie
    pinMode(LED_PIN_ROUGE, OUTPUT);
    pinMode(LED_PIN_BLEU, OUTPUT);
    pinMode(LED_PIN_BLEU_2, OUTPUT);
    pinMode(VIBREUR_PIN, OUTPUT);
    eteindreLED();  // S'assurer que la LED est éteinte au départ
    digitalWrite(VIBREUR_PIN, LOW);  // S'assurer que le vibreur est éteint au départ

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
    if (alarmActive) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            // Alterner entre allumer les LEDs rouge et bleu
            static bool ledState = false;
            if (ledState) {
                allumerLEDRouge();
            } else {
                allumerLEDBleu();
            }
            ledState = !ledState;
        }
    }
}
