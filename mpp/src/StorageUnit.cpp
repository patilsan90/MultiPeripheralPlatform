#include <StorageUnit.h>
#include <JSONParser.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>
#include "error_codes.h"

void StorageUnit::init()
{
  Serial.println(F("Spiffs Storage Unit initializing....."));
  SPIFFS.begin();
}

int StorageUnit::convertStringToObj(String input)
{
  String msg;

  Serial.println(F("*********"));
  Serial.println(input);
  Serial.println(F("*********"));

  Serial.println(F("*****PARSING START****"));

  JSONParser *parser = new JSONParser();
  parser->parse(input);

  Serial.printf("Total JSON pairs:: %d\n ", parser->total_pairs);

  int config_details_counter = 0;
  for (int i = 0; i < parser->total_pairs; i++)
  {
    if (strcmp(parser->pairs[i].key, "owner_id") == 0)
    {
      String currentOwner = this->getOwner();
      String val = (String)parser->pairs[i].val;
      currentOwner.trim();
      val.trim();
      if (currentOwner.compareTo("NO_OWNER") != 0)
        if (currentOwner.compareTo(val) != 0)
        {
          Serial.println("Current Owner :: "+currentOwner);
          return CONFIGURATION_OWNER_MISMATCH;
        }

      config_details_counter++;
      this->owner_id = (String)parser->pairs[i].val;
    }

    else if (strcmp(parser->pairs[i].key, "wifi_ssid") == 0)
    {
      config_details_counter++;
      this->wifi_ssid = (String)parser->pairs[i].val;
    }
    else if (strcmp(parser->pairs[i].key, "wifi_psw") == 0)
    {
      config_details_counter++;
      this->wifi_psw = (String)parser->pairs[i].val;
    }
    else if (strcmp(parser->pairs[i].key, "transact_server_url") == 0)
    {
      config_details_counter++;
      this->server_url = (String)parser->pairs[i].val;
    }

    Serial.print(parser->pairs[i].key);
    Serial.print(" : ");
    Serial.print(parser->pairs[i].val);
    Serial.println();
  }
  Serial.println(F("******PARSING DONE ......"));

  if (config_details_counter >= 4)
    return CONFIGURATION_SUCCESS;

  return CONFIGURATION_INCOMPLETE_INFO;
}

int StorageUnit::saveConfiguration(String inputBuffer)
{
  this->init();
  int ret = this->convertStringToObj(inputBuffer);
  if (ret != CONFIGURATION_SUCCESS)
    return ret;

  File file;
  SPIFFS.format();
  Serial.println(F("Spiffs formatted"));

  // Open file for writing
  file = SPIFFS.open("/wifi_ssid.txt", "w");
  if (!file)
  {
    Serial.println(F("wifi_ssid write open failed"));
  }
  else
  {
    Serial.println(F("====== Writing to SPIFFS wifi_ssid file ========="));
    file.println((String)this->wifi_ssid);
  }
  file.close();

  // Open file for writing
  file = SPIFFS.open("/wifi_psw.txt", "w");
  if (!file)
  {
    Serial.println(F("wifi_psw write open failed"));
  }
  else
  {
    Serial.println(F("====== Writing to SPIFFS wifi_psw file ========="));
    file.println((String)this->wifi_psw);
  }
  file.close();
  // Open file for writing
  file = SPIFFS.open("/owner_id.txt", "w");
  if (!file)
  {
    Serial.println(F("owner_id write open failed"));
  }
  else
  {
    Serial.println(F("====== Writing to SPIFFS owner_id file ========="));
    file.println((String)this->owner_id);
  }
  file.close();
  // Open file for writing
  file = SPIFFS.open("/server_url.txt", "w");
  if (!file)
  {
    Serial.println(F("server_url write open failed"));
  }
  else
  {
    Serial.println(F("====== Writing to SPIFFS server_url file ========="));
    file.println((String)this->server_url);
  }
  file.close();
  return CONFIGURATION_SUCCESS;
}

void StorageUnit::loadConfiguration()
{
  String s;
  File file;
  this->init();
  // Open file for Reading
  file = SPIFFS.open("/wifi_ssid.txt", "r");
  if (!file)
  {
    Serial.println(F("wifi_ssid Read open failed"));
  }
  else
  {
    Serial.println(F("====== Reading to SPIFFS wifi_ssid file ========="));
    this->wifi_ssid = file.readStringUntil('\n');
    this->wifi_ssid.trim();

    Serial.println(this->wifi_ssid);
  }
  file.close();

  // Open file for Reading
  file = SPIFFS.open("/wifi_psw.txt", "r");
  if (!file)
  {
    Serial.println(F("wifi_psw Read open failed"));
  }
  else
  {
    Serial.println(F("====== Reading to SPIFFS wifi_psw file ========="));
    this->wifi_psw = file.readStringUntil('\n'); //file.println((String)this->wifi_psw);
    this->wifi_psw.trim();
    Serial.println(this->wifi_psw);
  }
  file.close();
  // Open file for Reading
  file = SPIFFS.open("/owner_id.txt", "r");
  if (!file)
  {
    Serial.println(F("owner_id Read open failed"));
  }
  else
  {
    Serial.println(F("====== Reading to SPIFFS owner_id file ========="));
    this->owner_id = file.readStringUntil('\n'); // file.println((String)this->owner_id);
    Serial.println(this->owner_id);
    this->owner_id.trim();
  }
  file.close();
  // Open file for Reading
  file = SPIFFS.open("/server_url.txt", "r");
  if (!file)
  {
    Serial.println(F("server_url Read open failed"));
  }
  else
  {
    Serial.println(F("====== Reading to SPIFFS server_url file ========="));
    this->server_url = file.readStringUntil('\n'); //file.println((String)this->server_url);
    this->server_url.trim();
    Serial.println(this->server_url);
  }
  file.close();
}

OPERATION_MODE StorageUnit::getOperarionMode()
{
  if (this->wifi_ssid != NULL)
    return e_RECEPTION_MODE;
  return e_CONFIG_MODE;
}

String StorageUnit::ipToString(IPAddress ip)
{
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

bool StorageUnit ::saveIP()
{
  String local_ip = this->ipToString(WiFi.localIP());

  if (this->local_ip.compareTo(local_ip) != 0)
  {
    //ip is changed update to server also.
    this->local_ip = local_ip;
    return true;
  }
  return false;
}

String StorageUnit::getOwner()
{
  String s;
  File file;

  file = SPIFFS.open("/owner_id.txt", "r");
  if (!file)
  {
    Serial.println(F("owner_id write open failed"));
    file.close();
    return "NO_OWNER";
  }
  else
  {
    Serial.println(F("====== Reading to SPIFFS owner_id file ========="));
    String owner = file.readStringUntil('\n');
    owner.trim();
    file.close();

    if (0 == owner.length())
      return "NO_OWNER";

    return owner;
  }
}

String StorageUnit::getPlatformDeviceId()
{
  return "787878";
}