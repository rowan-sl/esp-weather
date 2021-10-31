#include <Arduino.h>

//might work, not tested, or test compiled, because i still need to figure things out
//made to run on esp2688 wiemos D-1 mini
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h>
#include "secret_stuff.h"

// TODO: check if this actualy works
//! GOD DANGIT C++/ARDUINO IS HORRIBLE!!!!!
//^ this is wrong

const int led = LED_BUILTIN;
const char* PARAM_INPUT_1 = "input1";

String inputMessage;
String lastMessage;

float tmpC;
float tmpF;
float rh;

AsyncWebServer server(80);

DHTesp dht;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "page not found");
}

void setup() {
  //start dht sensor
  dht.setup(D3, DHTesp::DHT11);
  
  pinMode(led,OUTPUT);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", "main page");
  });

  // retrieve temp data
  server.on("/temp", HTTP_GET, [] (AsyncWebServerRequest *request){
    String tmpFstr = "";
    tmpFstr.concat(tmpF);
    request->send(200, "text/plain", tmpFstr);
  });
  
  // retrieve humidity data
  server.on("/humididty", HTTP_GET, [] (AsyncWebServerRequest *request){
    String rhstr = "";
    rhstr.concat(rh);
    request->send(200, "text/plain", rhstr);
  });

  server.onNotFound(notFound);
  server.begin();
}

void getReadings(void) {
  static unsigned long lastTime = 0;
  const long interval = 2200;//interval in msc to run this code
  unsigned long now = millis();

  if ( now - lastTime > interval) {
     lastTime = now;
     //put code to run every [interval] milliseconds:
     rh = dht.getHumidity();
     tmpC = dht.getTemperature();
  
     if (isnan(tmpC) || isnan(rh)) {
       Serial.println(F("Failed to read from DHT sensor!"));
       return;
     }
     tmpF = (tmpC * 1.8) + 32;
  }
}

void loop() {
  getReadings();
  delay(5000);
}
