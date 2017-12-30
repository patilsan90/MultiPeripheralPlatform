#include <Arduino.h>
#include "StorageUnit.h"
#include "ConfigurationMode.h"
#include "Wifi.h"

StorageUnit *storageUnit = new StorageUnit();
ServerMode serverMode;
OPERATION_MODE mode;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);

    storageUnit->loadConfiguration();
    mode = storageUnit->getOperarionMode();

    //if (mode == e_CONFIG_MODE)
    {
        Serial.println(F("INIT in Config Mode"));
        serverMode.init();
    }
}

void loop()
{
  //  if (mode == e_CONFIG_MODE)
    {
        Serial.println(F("Working in Config Mode"));
        Serial.printf("Stations connected = %d \n", WiFi.softAPgetStationNum());
        serverMode.handleClient(); //Handle client requests
        delay(1000);
    }
//    else{
  //      Serial.println(F("Working in Reception Mode"));
   // }
}