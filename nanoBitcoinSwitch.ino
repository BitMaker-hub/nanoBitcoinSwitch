#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include "configs.h"
#include <ArduinoJson.h>

using WebServerClass = WebServer;

int balance;
int oldBalance;
bool down = false;

WebServerClass server;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  pinMode (switchPin, OUTPUT);

  Serial.println("BitcoinSwitch started......");
  Serial.print("   > Connecting to ssid: "); Serial.println(WIFI_SSID);
  Serial.print("   > Password: "); Serial.println(WIFI_PASSWORD);
  Serial.print("   > SwitchPin: ");Serial.println(switchPin);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
}

void loop() {
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[Error] - Failed to connect");
  }

  //CHECK IF LNURL type
  String lnurlP = LNURLP;
  Serial.println("CONNECTED [LNURL mode] - Waiting payment...");  
  if(lnurlP.substring(0, 5) == "LNURL"){
    checkBalance();
    oldBalance = balance;
    while((oldBalance + amount) > balance){
      checkBalance();
      delay(2000);
    }
    oldBalance = balance;
    Serial.println("Paid");
    digitalWrite(switchPin, HIGH);
    delay(timePin);
    digitalWrite(switchPin, LOW); 
    delay(2000);
  }
}

//////////////////NODE CALLS///////////////////

void checkBalance(){
  WiFiClientSecure client;
  client.setInsecure();
  //const char* lnbitsserver = lnbitsServer.c_str();
  //const char* invoicekey = invoiceKey.c_str();
  if (!client.connect(lnbitsServer, 443)){
    down = true;
    return;   
  }

  String url = "/api/v1/wallet";
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                "Host: " + lnbitsServer + "\r\n" +
                "X-Api-Key: "+ invoiceKey +" \r\n" +
                "User-Agent: ESP32\r\n" +
                "Content-Type: application/json\r\n" +
                "Connection: close\r\n\r\n");
   while (client.connected()) {
   String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    if (line == "\r") {
      break;
    }
  }
  String line = client.readString();
  Serial.println(line);
  StaticJsonDocument<500> doc;
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  int charBalance = doc["balance"];
  balance = charBalance;
}
