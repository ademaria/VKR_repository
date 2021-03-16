#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <SPI.h>
#include <LoRa.h>
#include "serverHtml.h"

//define the pins used by the transceiver module
#define ss 18     //was 5, CS on heltec
#define rst 14    //ok for heltec
#define dio0 26   //was 2

// Initialize network parameters
const char* ssid = "XaLeX's_Home";
const char* password = "1805lbtx";

//String for LoRa data recieved
String tmpLoRaData, parsedTemperature, parsedHumidity;

// Instantiate server objects
WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

void setup() {
    // Serial port for debugging purposes
  Serial.begin(115200);
  
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  
  // Connect to WiFi
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());

  // define the routes in which the server is accessible
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);  
  });

  // initialize server and websockets
  server.begin();
  webSocket.begin();

  // handling incoming messages on the websocket
  webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if(type == WStype_TEXT)
  {
    // processs any returned data
    Serial.printf("payload [%u]: %s\n", num, payload);
  }
}

void stringParsing()
{
  for (int i = 0; i < tmpLoRaData.length(); i++) 
  {
    if (tmpLoRaData.substring(i, i+1) == ";") 
    {
      parsedTemperature = tmpLoRaData.substring(0, i);
      parsedHumidity = tmpLoRaData.substring(i+1);
      break;
    }
  }
}

void loop() 
{
  webSocket.loop();
  server.handleClient(); 
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    // received a packet
    Serial.print("Received packet '");
    // read packet
    while (LoRa.available()) 
    {
      String LoRaData = LoRa.readString();
      Serial.print(LoRaData);
      tmpLoRaData = LoRaData;
      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());
    }
    stringParsing();
    String json = "{\"temperature\":";
    json += parsedTemperature;
    json += ",\"humidity\":";
    json += parsedHumidity;
    json += "}";
    Serial.println(json); // DEBUGGING
    webSocket.broadcastTXT(json.c_str(), json.length());
  }  
}
