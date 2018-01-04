#ifndef __PERIPHERAL__
#define __PERIPHERAL__

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "pin_configuration.h"

#define NO_ID ("NO_ID")

#define NO_RESPONSE ("NO_RESPONSE")

#define CMD_GET_DEVICE_UPDATES ("GDU")
class Peripheral
{

public:
  Peripheral()
  {
    perSerial = new HardwareSerial(1);
    perSerial->begin(19200, SERIAL_8N1, PIN_PERIPHERAL_RX, PIN_PERIPHERAL_TX);
  }
  HardwareSerial *perSerial;

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
