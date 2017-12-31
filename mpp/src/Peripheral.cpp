
#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Peripheral.h>
#include <pin_configuration.h>

#define DEVICE_ID_LEN (8)         //it is 8 bytes
#define PARENT_DEVICE_ID_LEN (17) //it is 8 bytes

char dev_id[DEVICE_ID_LEN];

#define RESP_WAIT_TIME_OUT 3000 //in ms
//static SoftwareSerial *perSerial = new SoftwareSerial(RX, TX, false, 256);

bool Peripheral::isConnected()
{
    //return digitalRead(this->pin_number))==1?true:false;
    return true;
}

String Peripheral::getDeviceType()
{
    String cmd = "{\"GDT\":\"\"}";
    perSerial->flush();
    perSerial->println(cmd);
    Serial.println(cmd);
    int i = 0;
    while (!perSerial->available())
    {
        delay(1);
        if (i++ >= RESP_WAIT_TIME_OUT)
            return "NO_RESPONSE";
    }

    String str = "";
    while (perSerial->available())
    {
        str = perSerial->readString();
    }
    return str;
}

String Peripheral::setDeviceRegID(String reg_id)
{
    String status;
    String cmd = "{\"SDI\":\"" + reg_id + "\"}";
    perSerial->flush();
    perSerial->println(cmd);
    Serial.println(cmd);

    int i = 0;
    while (!perSerial->available())
    {
        delay(1);
        if (i++ >= RESP_WAIT_TIME_OUT)
            return "NO_RESPONSE";
    }
    while (perSerial->available())
    {
        status = perSerial->readString();
    }
    return status;
}

int Peripheral ::getRTS()
{
    return digitalRead(PIN_RTS);
}

String Peripheral::getDeviceRegID()
{
    String cmd = "{\"GDI\":\"\"}";

    perSerial->flush();

    perSerial->println(cmd);
    Serial.println(cmd);

    int i = 0;
    while (!perSerial->available())
    {
        delay(1);
        if (i++ >= RESP_WAIT_TIME_OUT)
            return "NO_RESPONSE";
    }

    String str = "";
    while (perSerial->available())
    {
        str = perSerial->readString();
    }
    return str;
}
String Peripheral::getDeviceStatus()
{
    return "Connected yeye";
}

String Peripheral::sendUpdates(String cmd)
{
    String status;
    perSerial->flush();
    perSerial->println(cmd);
    Serial.println("Sending command :: " + cmd);
    int i = 0;
    while (!perSerial->available())
    {
        delay(1);
        if (i++ >= RESP_WAIT_TIME_OUT)
            return "NO_RESPONSE";
    }
    while (perSerial->available())
    {
        status = perSerial->readString();
    }
    return status;
}

String Peripheral::getUpdates()
{

    String cmd = "{\"";
    cmd += CMD_GET_DEVICE_UPDATES;
    cmd += "\":}";

    String status;
    perSerial->flush();
    perSerial->println(cmd);
    Serial.println("Sending command :: " + cmd);
    int i = 0;
    while (!perSerial->available())
    {
        delay(1);
        if (i++ >= RESP_WAIT_TIME_OUT)
            return "NO_RESPONSE";
    }
    while (perSerial->available())
    {
        status = perSerial->readString();
    }
    return status;
}