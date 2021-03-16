/**
* ESP-NOW
*
* Sender
*/
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
 
#define DHTTYPE DHT11
#define DHTPin D6
 
uint8_t peer1[] = {0x7C, 0x9E, 0xBD, 0xFC, 0x17, 0xD0};
 
typedef struct message {
  float temperature;
  float humidity;
};
 
struct message myMessage;
 
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
 
void onSent(uint8_t *mac_addr, uint8_t sendStatus) {
 Serial.println("Status:");
 Serial.println(sendStatus);
}
 
void setup() {
 Serial.begin(115200);
 
 dht.begin();
 
 WiFi.mode(WIFI_STA);
 
 // Get Mac Add
 Serial.print("Mac Address: ");
 Serial.print(WiFi.macAddress());
 Serial.println("ESP-Now Sender");
 
 // Initializing the ESP-NOW
 if (esp_now_init() != 0) {
   Serial.println("Problem during ESP-NOW init");
   return;
 }
 
 esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
 // Register the peer
 Serial.println("Registering a peer");
 esp_now_add_peer(peer1, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
 Serial.println("Registering send callback function");
 esp_now_register_send_cb(onSent);
}
 
void loop() {
 myMessage.temperature = dht.readTemperature();
 myMessage.humidity = dht.readHumidity();
 
 Serial.println("Send a new message");
 esp_now_send(NULL, (uint8_t *) &myMessage, sizeof(myMessage));
 
 delay(10000);
}
