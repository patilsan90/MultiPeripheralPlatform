#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <HTTPClient.h>
#include <ReceptionMode.h>
#include <Peripheral.h>
#include "pin_configuration.h"
#include "json_id_parser.h"

WiFiServer server(137);

static StorageUnit *storage; // =new StorageUnit();

static Peripheral *peripheral = new Peripheral();

boolean is_list_sent = false;
boolean is_ip_send_success = false;

IPAddress no_IP(0, 0, 0, 0);

boolean all_registrations_success = false;

String updates_from_peripherals_to_server;

void ReceptionMode ::init(StorageUnit *in_storage)
{
  storage = in_storage;
  WiFi.setHostname("SinglePeripheralPlatform");
  WiFi.mode(WIFI_AP_STA);
  //WiFi.mode(WIFI_STA);
  //storage->wifi_ssid.trim();
 // storage->wifi_psw.trim();

  WiFi.begin(storage->wifi_ssid.c_str(), storage->wifi_psw.c_str());

  // TODO:: Below line is for testing purpose, and need to be removed in actual code.
  storage->server_url = "http://192.168.1.5:8090/smart_home";

  /*
   *  Below commented code line is kept for future reference and debug purpose.
   *  
   *  WiFi.begin("Techies2.4", "passwordis1to8");
   *
   */
  // Start TCP server.
  //server.begin();
  for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
    is_connected[i] = false;
}

void ReceptionMode ::start()
{

  // if (WiFi.status() == WL_CONNECTED)
  if (WiFi.localIP() != no_IP)
  {
    Serial.println("Connected to WiFi ********************************");
    Serial.printf("WIFI Status :: %d\n", WiFi.status());
    Serial.printf("is_list_sent :: %d\n", is_list_sent);

    gatherPeripherals();

    Serial.printf("all_registrations_success :: %d\n", all_registrations_success);
    if (all_registrations_success == false)
    {
      for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
      {
        if (true == is_connected[i])
        {
          this->selectLine(i);
          String dev_id = peripheral->getDeviceRegID();
          Serial.println("Re-registering device ID ");
          Serial.println(dev_id);
          verifyAndRegisterPeripheral(dev_id, i, 0);
        }
      }
    }

    if (checkForRTS())
    {
      sendTriggersListToServer();
    }

    if (is_list_sent == false)
      sendPeripheralsListToServer();

    if (isIPChanged() == true || is_ip_send_success == false)
      updateLocalIPToServer();

    if (is_list_sent == true && is_ip_send_success == true)
    {
      String payload = getUpdatesFromServer();

      Serial.println(F("**********************************************"));
      Serial.printf("Updates from Server:: Payload ::\n");
      Serial.println(payload);
      Serial.println(F("**********************************************"));

      if (payload.compareTo("NO_UPDATE") != 0)
        updatePeripherals(payload);
    }
  }
  else
  {
    Serial.println("Not Connected to WiFi ********************************");
    Serial.printf("WIFI Status :: %d\n", WiFi.status());
    storage->saveIP();
    Serial.println("My IP:: ");
    Serial.println(storage->local_ip);
  }
  delay(1000); //Send a request every 1 second
}

boolean ReceptionMode ::checkForRTS()
{
  updates_from_peripherals_to_server = "\"alPeripherals\":[";

  boolean first_instance = true;

  for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
  {
    this->selectLine(i);
    rts[i] = peripheral->getRTS();
    Serial.printf("RTS ---------------------------------------- %d\n", rts[i]);
    if (rts[i] == 1)
    {
      if (first_instance == false)
      {
        updates_from_peripherals_to_server += ",";
      }
      first_instance = false;
      String triggerUpdates = peripheral->getUpdates();
      Serial.printf("RTS updates start----------------------------------------\n");
      Serial.println(triggerUpdates);
      Serial.printf("RTS updates end----------------------------------------\n");
      updates_from_peripherals_to_server += triggerUpdates;
    }
  }

  updates_from_peripherals_to_server += "]";

  if (false == first_instance)
    return true;

  return false;
}

void ReceptionMode ::sendTriggersListToServer()
{
  String url = (String)storage->server_url + "" + storage->send_trigger_url;
  sendDevPerListToServer(url, updates_from_peripherals_to_server);
}

void ReceptionMode ::sendPeripheralsListToServer()
{
  String url = (String)storage->server_url + "" + storage->update_peripheral_list_url;

  String peripheralsList = "\"alPeripherals\":[";
  for (int i = 0; i < (MAX_NUMBER_OF_PERIPHERALS); i++)
  {
    if (true == is_connected[i])
    {
      connected_dev_type[i].trim();
      peripheralsList += "{\"pid\":" + connected_dev_ids[i] + ",\"pty\":\"" + connected_dev_type[i] + "\"";

      if ((i + 1) == (MAX_NUMBER_OF_PERIPHERALS))
        peripheralsList += "}";
      else
        peripheralsList += "},";
    }
  }
  peripheralsList += "]";

  sendDevPerListToServer(url, peripheralsList);
}

void ReceptionMode ::sendDevPerListToServer(String url, String peripheralsList)
{
  String mac = WiFi.macAddress();
  String device = "{\"device_id\":" + storage->getPlatformDeviceId();
  device += ",\"device_phy_id\":\"" + mac + "\"";
  device += ",\"dev_mac_addr\":\"" + mac + "\",";
  device += peripheralsList;
  device += "}";
  Serial.println("******************Device Peripheral List Object***********************");
  Serial.println(device);

  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(device);

  //  payload = http.getString(); Not required                                     //Get the response payload
  Serial.println("Update Peripheral list Request URL:: " + url); //Print Request URL
  Serial.println("Request Data:: " + device);                    //Print Request Data
  Serial.printf("HTTP code:: %d\n", httpCode);                   //Print HTTP return code
  // Serial.println("Response Payload :: " + payload); Not Required              //Print request response payload
  http.end(); //Close connection

  is_list_sent = false;
  if (HTTP_CODE_OK == httpCode)
    is_list_sent = true;
}

boolean ReceptionMode::isNewPeripheralConnected(int index)
{
  selectLine(index);
  boolean status = digitalRead(PIN_IS_CONNECTED) == 1 ? true : false;

  if (false == is_connected[index])
  {
    if (true == status)
    {
      //New device found
      is_connected[index] = true;
      return true;
    }
  }
  else
    is_connected[index] = status;
  return false;
}

void ReceptionMode ::selectLine(int index)
{
  digitalWrite(PIN_MUX_SELECTOR_LINE_0, index & 0x1);
  digitalWrite(PIN_MUX_SELECTOR_LINE_1, index & 0x2);
  digitalWrite(PIN_MUX_SELECTOR_LINE_2, index & 0x4);
  digitalWrite(PIN_MUX_SELECTOR_LINE_3, index & 0x8);
}

void ReceptionMode ::gatherPeripherals()
{
  boolean new_found_indices[MAX_NUMBER_OF_PERIPHERALS];
  boolean found_new = false;
  // scan for is_connected ..
  for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
  {
    new_found_indices[i] = isNewPeripheralConnected(i);
    if (new_found_indices[i] == true)
      found_new = true;
  }

  if (true == found_new)
  {
    is_list_sent = false;
    for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
    {
      if (true == new_found_indices[i])
      {
        this->selectLine(i);

        String dev_id = peripheral->getDeviceRegID();
        Serial.println("Get device ID ");
        Serial.println(dev_id);
        dev_id.trim();
        verifyAndRegisterPeripheral(dev_id, i, 3);
      }
    }
  }
}

void ReceptionMode::verifyAndRegisterPeripheral(String dev_id, int index, int re_attempt_times)
{
  if (dev_id.compareTo(NO_ID) == 0)
  {
    //request server for new ID
    String new_reg_id = "NO_ID";

    String mac = WiFi.macAddress();
    String device = "{ \"device_id\":" + storage->getPlatformDeviceId();
    device += ",\"device_phy_id\":\"" + mac + "\"";
    device += ",\"dev_mac_addr\":\"" + mac + "\"";
    device += "}";
    Serial.println("******************verifyAndRegisterPeripheral Object***********************");
    Serial.println(device);

    HTTPClient http;
    String url = storage->server_url + "" + storage->register_peripheral_url;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(device);

    new_reg_id = http.getString();                                  //Get the response payload
    Serial.println("register_peripheral_url Request URL:: " + url); //Print Request URL
    Serial.println("Request Data:: " + device);                     //Print Request Data
    Serial.printf("HTTP code:: %d\n", httpCode);                    //Print HTTP return code
    Serial.println("Response Payload new_reg_id :: " + new_reg_id); //Print request response payload
    http.end();                                                     //Close connection

    if (HTTP_CODE_OK == httpCode)
    {
      /* Once received new ID set this id to peripheral. */
      String status = peripheral->setDeviceRegID(new_reg_id);
      this->connected_dev_ids[index] = new_reg_id;

      connected_dev_type[index] = peripheral->getDeviceType();
      Serial.println("Get device Type");
      Serial.println(connected_dev_type[index]);
      all_registrations_success = true;
    }
    else
    {
      /*
       * What if registration of the device is failed. 
       * Do three attempts of re-registration.
       * If Still did not worked, then restart ESP, it is a problem of Server.
       */
      if (0 != re_attempt_times)
      {
        Serial.println(F("###################### Reattempt Device registration ############################"));
        verifyAndRegisterPeripheral(dev_id, index, --re_attempt_times);
      }
      else
      {
        Serial.println(F("###################### Global Reattempt Device registration ############################"));
        all_registrations_success = false;
      }
    }
  }
  else if (dev_id.compareTo(NO_RESPONSE) == 0)
  {
    /*
     * Try to switch its power off-on
     * TO-DO ::Might need mux/ voltage regulator to handle power of perticular Peripheral.
     */
    for (int i = 0; i < MAX_NUMBER_OF_PERIPHERALS; i++)
      this->is_connected[i] = false;

    Serial.println("No Response from Peripheral,switching On/Off peripherals, encountered at index:" + index);
    digitalWrite(PIN_PERIPHERAL_ACTIVATE_VCC, 0);
    delay(1000);
    Serial.println("Waiting to get peripherals enabled.");
    digitalWrite(PIN_PERIPHERAL_ACTIVATE_VCC, 1);
    delay(3000);
  }
  else
  {
    all_registrations_success = true;
    Serial.println("Device is already registered, encountered at index:" + index);
    this->connected_dev_ids[index] = dev_id;
    connected_dev_type[index] = peripheral->getDeviceType();
    Serial.println("Get device Type");
    Serial.println(connected_dev_type[index]);
  }
}

void ReceptionMode ::printWiFiStatus()
{
  Serial.print(F("Connected to "));
  //Serial.println(ssid);
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());
}

bool ReceptionMode ::isIPChanged()
{
  return storage->saveIP();
}

void ReceptionMode ::updateLocalIPToServer()
{
  String payload;
  HTTPClient http;
  String url = storage->server_url + "" + storage->update_ip_url;
  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  String ip = ipToString(WiFi.localIP());

  String device = "{ \"device_id\":" + storage->getPlatformDeviceId();
  device += ",\"device_phy_id\":\"" + WiFi.macAddress() + "\"";
  device += ",\"dev_mac_addr\":\"" + WiFi.macAddress() + "\"";
  device += ",\"local_ip_addr\":\"" + ip + "\"";
  device += "}";

  int httpCode = http.POST(device);
  payload = http.getString();                                 //Get the response payload
  Serial.println("UPDATE_IP Request URL:: " + url);           //Print Request URL
  Serial.println("UPDATE_IP Request Data:: " + device);       //Print Request Data
  Serial.printf("UPDATE_IP HTTP code:: %d\n", httpCode);      //Print HTTP return code
  Serial.println("UPDATE_IP Response Payload :: " + payload); //Print request response payload
  http.end();                                                 //Close connection

  is_ip_send_success = false;
  if (HTTP_CODE_OK == httpCode)
    is_ip_send_success = true;
}

void ReceptionMode::updatePeripherals(String input)
{
  Serial.println("Update Peripherals");
  int ret = parse(input.c_str());
  Serial.printf("############# !!!!!!!!!!!!!!!!!!!!!!!!total Ret val periphrals info received :: %d\n", ret);
  Serial.printf("############# total periphrals info received :: %d\n", total_peripherals);
  for (int i = 0; i < total_peripherals; i++)
  {
    Serial.printf("-> %s\n", peripherals[i].json_data);
    Serial.printf("-> %s\n", peripherals[i].id);
    String strId = (String)peripherals[i].id;
    strId.trim();
    for (int j = 0; j < MAX_NUMBER_OF_PERIPHERALS; j++)
    {
      if (connected_dev_ids[j].compareTo(strId) == 0)
      {
        Serial.printf("############# Peripheral ID matched at :: %d\n", j);
        selectLine(j);
        String status = peripheral->sendUpdates(peripherals[i].json_data);
        Serial.print("############# Peripheral Return Status :: ");
        Serial.print(status);
      }
    }
  }
  //use vijen provided JSON parser;
  // for all splitted string send updates to peripherals
}

String ReceptionMode ::getUpdatesFromServer()
{
  String payload;
  String device = "{ \"device_id\":" + storage->getPlatformDeviceId();
  device += ",\"device_phy_id\":\"" + WiFi.macAddress() + "\"";
  device += ",\"dev_mac_addr\":\"" + WiFi.macAddress() + "\"";
  device += "}";

  HTTPClient http;
  String url = storage->server_url + "" + storage->get_updates_url;
  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(device);
  payload = http.getString();                       //Get the response payload
  Serial.println("Request URL:: " + url);           //Print Request URL
  Serial.println("Request Data:: " + device);       //Print Request Data
  Serial.printf("HTTP code:: %d\n", httpCode);      //Print HTTP return code
  Serial.println("Response Payload :: " + payload); //Print request response payload
  http.end();                                       //Close connection

  if (payload.length() <= 5)
    return "NO_UPDATE";
  return (payload);
}

String ReceptionMode ::ipToString(IPAddress ip)
{
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
