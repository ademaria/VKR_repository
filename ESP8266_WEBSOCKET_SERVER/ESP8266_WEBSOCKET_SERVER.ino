#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <espnow.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>
#include <AsyncElegantOTA.h>

// Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WiFiMulti wifiMulti; 

// Replace with your network credentials
const char* ssid = "Smart Home Access Point";
const char* password = "";

const char* mdnsName = "smarthome"; // Domain name for the mDNS responder

// Create AsyncWebServer object on port 80 and WebSocketsServer object on port 81 
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Set number of outputs
#define NUM_OUTPUTS  4

// Assign each GPIO to an output
int outputGPIOs[NUM_OUTPUTS] = {2, 4, 12, 14};

// Structure example to send data (Must match the receiver structure!!!)
typedef struct struct_message {
    int ID_number;
    float temperature;
    int humidity;
    int ambientLight;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) 
{
  Serial.println("Message received.");
  // We don't use mac to verify the sender
  // Let us transform the incomingData into our message structure
   memcpy(&myData, incomingData, sizeof(myData));
   Serial.println("=== Data ===");
   Serial.print("Mac address: ");
   for (int i = 0; i < 6; i++) 
   {
       Serial.print("0x");
       Serial.print(mac[i], HEX);
       Serial.print(":");
   }
   
  //structuring the json string to send
  String json = "{\"id\":";
  json += String(myData.ID_number);
  json += ",\"temperature\":";
  json += String(myData.temperature);
  json += ",\"humidity\":";
  json += String(myData.humidity);
  json += ",\"ambientLight\":";
  json += String(myData.ambientLight);
  json += "}"; 

  //send the json string over WebSocket
  webSocket.broadcastTXT(json.c_str(), json.length());
  
  //DEBUGGING 
  Serial.println(json); 
  Serial.print("Message length: "); 
  Serial.println(json.length()); 
  Serial.println();   
}

String getOutputStates(){
  JSONVar myArray;
  for (int i =0; i<NUM_OUTPUTS; i++){
    myArray["gpios"][i]["output"] = String(outputGPIOs[i]);
    myArray["gpios"][i]["state"] = String(digitalRead(outputGPIOs[i]));
  }
  String jsonString = JSON.stringify(myArray);
  return jsonString;
}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "states") == 0) {
      notifyClients(getOutputStates());
    }
    else{
      int gpio = atoi((char*)data);
      digitalWrite(gpio, !digitalRead(gpio));
      notifyClients(getOutputStates());
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), 
      client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

/****************************************** VOID SETUP *******************************************/
void startWiFi(){
  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started\r\n");

  // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");   
//  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
//  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting"); 
  // Wait for the Wi-Fi to connect
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {  
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if(WiFi.softAPgetStationNum() == 0) {      // If the ESP is connected to an AP
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());             // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  } else {                                   // If a station is connected to the ESP SoftAP
    Serial.print("Station connected to ESP8266 AP");
  }
  Serial.println("\r\n");
}

void startESPNOW(){
  // Initializing the ESP-NOW
  if (esp_now_init() != 0){
   Serial.println("Problem during ESP-NOW init");
   return;
  } Serial.println("ESP-NOW initialized");
  //  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void startLittleFS(){
  // Initialize LittleFS
  if (!LittleFS.begin()){
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");
}

void startWebSocket() {
  webSocket.begin();
  ws.onEvent(onEvent);
  server.addHandler(&ws); 
  Serial.println("WebSocket server started.");
}

void startMDNS() {
  MDNS.begin(mdnsName); // start the multicast domain name server                        
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html",false);
  });
  server.serveStatic("/", LittleFS, "/");
  // Start server
  server.begin();
  Serial.println("HTTP server started.");
}

void setup(){
  // Set GPIOs as outputs
  for (int i =0; i<NUM_OUTPUTS; i++){
    pinMode(outputGPIOs[i], OUTPUT);
  } 
  Serial.begin(115200); // Start the serial monitor for debugging purposes
  startWiFi();          // Start a Wi-Fi access point
  startESPNOW();        // Start the ESP-NOW for recieving 
  startLittleFS();      // Start the LittleFS filesystem
  startWebSocket();     // Start a WebSocket server
  startMDNS();          // Start the mDNS responder
  startServer();        // Start a HTTP server with a file read handler
  AsyncElegantOTA.begin(&server); // Start the ElegantOTA
}
/****************************************** VOID SETUP *******************************************/

void loop() {
  webSocket.loop();
  ws.cleanupClients();
  MDNS.update();
  AsyncElegantOTA.loop();
}
