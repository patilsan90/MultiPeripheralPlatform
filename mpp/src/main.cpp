#include <Arduino.h>
#include "StorageUnit.h"
#include "ConfigurationMode.h"
#include "Wifi.h"
#include "pin_configuration.h"

StorageUnit *storageUnit = new StorageUnit();
ServerMode serverMode;
OPERATION_MODE mode;
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(PIN_PERIPHERAL_ACTIVATE_VCC, OUTPUT);
    pinMode(PIN_IS_CONNECTED, INPUT);

    pinMode(PIN_MUX_SELECTOR_LINE_0, OUTPUT);
    pinMode(PIN_MUX_SELECTOR_LINE_1, OUTPUT);
    pinMode(PIN_MUX_SELECTOR_LINE_2, OUTPUT);
    pinMode(PIN_MUX_SELECTOR_LINE_3, OUTPUT);

    digitalWrite(PIN_PERIPHERAL_ACTIVATE_VCC, 0);
    delay(3000);
    digitalWrite(PIN_PERIPHERAL_ACTIVATE_VCC, 1);

    storageUnit->loadConfiguration();
    mode = storageUnit->getOperarionMode();

    if (mode == e_CONFIG_MODE)
    {
        Serial.println(F("INIT in Config Mode"));
        serverMode.init();
    }
}

void loop()
{
    if (mode == e_CONFIG_MODE)
    {
        Serial.println(F("Working in Config Mode"));
        Serial.printf("Stations connected = %d \n", WiFi.softAPgetStationNum());
        serverMode.handleClient(); //Handle client requests
    }
    else
    {
        Serial.println(F("Working in Reception Mode"));
    }
    delay(1000);
}