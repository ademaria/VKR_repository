#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the transceiver module
#define ss 18     //was 5, cs on heltec
#define rst 14    //ok for heltec
#define dio0 26   //was 2

unsigned int counter;
bool messageRecieved;

typedef struct message 
{
  float temperature;
  float humidity;
};
 
struct message myMessage;
 
void onDataReceiver(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  Serial.println("Message received.");
  // We don't use mac to verify the sender
  // Let us transform the incomingData into our message structure
   memcpy(&myMessage, incomingData, sizeof(myMessage));
   Serial.println("=== Data ===");
   Serial.print("Mac address: ");
   for (int i = 0; i < 6; i++) 
   {
       Serial.print("0x");
       Serial.print(mac[i], HEX);
       Serial.print(":");
   }  
   Serial.print("\n\nTemperature: ");
   Serial.println(myMessage.temperature);
   Serial.print("\nHumidity: ");
   Serial.println(myMessage.humidity);
   Serial.println();
   messageRecieved = true;
}
 
void setup() {
 Serial.begin(115200);
 WiFi.mode(WIFI_STA);
 
 // Get Mac Add
 Serial.print("Mac Address: ");
 Serial.print(WiFi.macAddress());
 Serial.println("ESP32 ESP-Now Broadcast");
 
 // Initializing the ESP-NOW
 if (esp_now_init() != 0) 
 {
   Serial.println("Problem during ESP-NOW init");
   return;
 }
 esp_now_register_recv_cb(onDataReceiver);
 
/*  LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa LoRa  */

  Serial.println("LoRa Sender");
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) 
  {
    Serial.println(".");
    delay(500);
  }
  // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

 
void loop() {
  if (messageRecieved) 
  {
    Serial.print("Sending packet: ");
    Serial.println(counter);
    
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(myMessage.temperature);
    LoRa.print(";");
    LoRa.print(myMessage.humidity);
    LoRa.endPacket();
    counter++;
    
    messageRecieved = false;
  } 
}
