#include <Arduino.h>
#include "StorageUnit.h"

StorageUnit *storageUnit = new StorageUnit();
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    String msg = "{\"owner_id\":\"9423306174\"}";
    storageUnit->saveConfiguration(msg);
}

void loop()
{
    // put your main code here, to run repeatedly:
    Serial.println("Testing code");
    storageUnit->loadConfiguration();
    Serial.println(storageUnit->owner_id);

    delay(2000);
}