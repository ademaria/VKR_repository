/*
  by Alessandro De Maria
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xD8, 0xBF, 0xC0, 0x14, 0x0C, 0x6C};

// Structure example to send data (Must match the receiver structure!!!)
typedef struct struct_message {
    // Set the ID number for the sender  
    int ID_number = 2;
    float temperature;
    int humidity;
    int ambientLight;
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

 // Insert your SSID
constexpr char WIFI_SSID[] = "XaLeX's_Home";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
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

  int32_t channel = getWiFiChannel(WIFI_SSID);
//  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
//  WiFi.printDiag(Serial); // Uncomment to verify channel change after
  
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
    // read the input on analog pin 0 and adjust it to percents:
    myData.ambientLight = map(analogRead(A0), 0, 1024, 0, 100);
    // read the input on DHT11 sensor:
    myData.temperature = dht.readTemperature();
    myData.humidity = dht.readHumidity();
    
    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));  

    Serial.println(myData.ID_number);
    Serial.println(myData.temperature);
    Serial.println(myData.humidity);
    Serial.println(myData.ambientLight);
}

void loop() {
  sendMessage();  
  ESP.deepSleep(30e6); //Send message every 1 minute(s)   
} 
