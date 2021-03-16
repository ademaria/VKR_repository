/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp8266-nodemcu-arduino-ide/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  Modified by Alessandro De Maria
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xD8, 0xBF, 0xC0, 0x14, 0x7D, 0xF3};
// Set the ID number for the sender
const int ID_number = 1;

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id, a, b;
    float c;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("Initialized");
  // Init DHT sensor
  dht.begin();
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void sendMessage() {
    // Set values to send  
    myData.id = ID_number;
    // read the input on analog pin 0 and adjust it to percents:
    myData.a = map(analogRead(A0), 0, 1024, 0, 100);
    myData.b = dht.readHumidity();
    myData.c = dht.readTemperature();
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    Serial.print("Humidity: ");
    Serial.println(myData.b);  
    Serial.print("Temperature: ");
    Serial.println(myData.c);  
}

void loop() {
  sendMessage();  
  ESP.deepSleep(300e6);   
} 
