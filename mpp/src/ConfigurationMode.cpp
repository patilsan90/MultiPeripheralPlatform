#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ConfigurationMode.h>
#include <WiFiClient.h>
#include "ESP32WebServer.h"
#include <WiFi.h>
#include <JSONParser.h>
#include <StorageUnit.h>
#include "error_codes.h"

ESP32WebServer config_server(1337);
IPAddress apIP(10, 10, 10, 1); // Private network address: local & gateway

static void handle_config_request()
{
  //config_server.sendHeader("Connection", "close");
  //config_server.sendHeader("Access-Control-Allow-Origin", "*");
  //config_server.send(200, "text/plain", "New hello from esp8266!");

  if (config_server.hasArg("plain") == true)
  {
    String response;
    String message = config_server.arg("plain");

    StorageUnit *storage = new StorageUnit();

    switch (storage->saveConfiguration(message))
    {
    case CONFIGURATION_SUCCESS:
      Serial.println(F("Success: Device configured successfully."));
      storage->loadConfiguration();
      response = "{ \"is_success\":\"true\", \"mac_addr\": \"";
      response += WiFi.macAddress();
      response += "\"}";
      config_server.send(200, "text/plain", response);
      delay(2000);
      ESP.restart();
      break;

    case CONFIGURATION_INCOMPLETE_INFO:
      Serial.println(F("Error: Incomplete information received."));
      response = "{ \"is_success\":\"false\"}";
      config_server.send(200, "text/plain", response);
      break;

    case CONFIGURATION_OWNER_MISMATCH:
      Serial.println(F("Error: You are not owner of this device."));
      response = "{ \"is_success\":\"false\", \"message\":\"You are not owner of this device.\"}";
      config_server.send(200, "text/plain", response);
      break;

    default:
      Serial.println(F("Error: incomplete Data received"));
    }
  }
}

void ServerMode ::closeAPMode()
{
  config_server.close();
  config_server.stop();
}

void ServerMode ::init()
{
  ServerMode serverMode;
  Serial.println(F("In Configuration mode"));
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // subnet FF FF FF 00

  String mac = WiFi.macAddress();

  String ap_name = "SmartHomeDevice-" + (String)mac.charAt(0) + (String)mac.charAt(5) +
                   (String)mac.charAt(mac.length() - 2) + (String)mac.charAt(mac.length() - 1);

  boolean result = WiFi.softAP(ap_name.c_str(), "transactsmarthome");
  if (result == true)
  {
    Serial.println(F("Smart home configuration mode is Ready"));
  }
  else
  {
    Serial.println(F("Smart home configuration mode Failed!"));
  }

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print(F("HotSpt IP:"));
  Serial.println(myIP);

  config_server.on("/smart_home/device/configure", handle_config_request); //Which routine to handle at root location

  config_server.begin(); //Start server
  Serial.println(F("HTTP server started"));
}

void ServerMode::handleClient()
{
  config_server.handleClient();
}