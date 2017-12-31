#ifndef __PERIPHERAL__
#define __PERIPHERAL__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <SoftwareSerial.h>

#define RX (13) //(14) // *** PB, Pin 0
#define TX (15) //(12) // *** PB, Pin 3

#define NO_ID ("NO_ID")

#define NO_RESPONSE ("NO_RESPONSE")

#define CMD_GET_DEVICE_UPDATES ("GDU")
class Peripheral
{

public:
  Peripheral()
  {
    perSerial = new SoftwareSerial(RX, TX, false, 256);
    perSerial->begin(19200);
  }
  SoftwareSerial *perSerial;

  bool isConnected();
  String getDeviceRegID();
  String setDeviceRegID(String reg_id);

  String getDeviceType();
  String getDeviceStatus();

  String sendUpdates(String cmd);
  String getUpdates();

  int getRTS();
};

#endif
