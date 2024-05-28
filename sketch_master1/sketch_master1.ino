#include <esp_now.h>
#include <WiFi.h>

#define SERIAL_BAUD 115200
#define MQ2_PIN 34
#define LED_ROUGE_PIN 26
#define LED_VERTE_PIN 27
#define LED_BLEU_PIN 25
#define BUZZER_PIN 33

const int seuilDeDetection = 50;  // Définir un seuil approprié pour la détection de fumée
const int delaiDetection = 10000;  // Délai en millisecondes avant de revérifier après détection de fumée

uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x7B, 0x7C, 0xF0}; // Remplacer par l'adresse MAC de la carte esclave

typedef struct struct_message {
    bool alarm;  // Booléen pour représenter l'état de l'alarme
} struct_message;

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nMaster packet sent: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void joue_melodie_ambulance() {
    tone(BUZZER_PIN, 900);  // Allume le buzzer à une fréquence de 900 Hz
    delay(500);
    tone(BUZZER_PIN, 600);  // Change la fréquence à 600 Hz
    delay(500);
}

void setup() {
    Serial.begin(SERIAL_BAUD);

    pinMode(MQ2_PIN, INPUT);
    pinMode(LED_ROUGE_PIN, OUTPUT);
    pinMode(LED_VERTE_PIN, OUTPUT);
    pinMode(LED_BLEU_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    Serial.print("Transmitter initialized: ");
    Serial.println(WiFi.macAddress());

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memset(&peerInfo, 0, sizeof(peerInfo));
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    int sensorValue = analogRead(MQ2_PIN);
    Serial.print("Valeur analogique du capteur MQ2: ");
    Serial.println(sensorValue);

    if (sensorValue > seuilDeDetection) {
        digitalWrite(LED_VERTE_PIN, LOW);
        digitalWrite(LED_ROUGE_PIN, HIGH);
        myData.alarm = true;

        // Envoyer l'état d'alarme à l'esclave
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }

        // Jouer la mélodie de l'ambulance pendant que l'alarme est active
        unsigned long startTime = millis();
        while (millis() - startTime < delaiDetection) {
            joue_melodie_ambulance();
        }
    } else {
        digitalWrite(LED_VERTE_PIN, HIGH);
        digitalWrite(LED_ROUGE_PIN, LOW);
        noTone(BUZZER_PIN);  // S'assurer que le buzzer est éteint
        myData.alarm = false;

        // Envoyer l'état d'alarme à l'esclave
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result == ESP_OK) {
            Serial.println("Sent with success");
        } else {
            Serial.println("Error sending the data");
        }

        delay(1000);  // Attendre 1 seconde avant la prochaine lecture
    }
}
