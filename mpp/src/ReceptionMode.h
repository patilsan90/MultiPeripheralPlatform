#ifndef __RECEPTION_MODE__
#define __RECEPTION_MODE__

#include <ESP8266WebServer.h>
#include "StorageUnit.h"

//#define MAX_NUMBER_OF_PERIPHERALS (16)
#define MAX_NUMBER_OF_PERIPHERALS (1)

class ReceptionMode
{
public:
  String connected_dev_ids[MAX_NUMBER_OF_PERIPHERALS];
  String connected_dev_type[MAX_NUMBER_OF_PERIPHERALS];
  boolean is_connected[MAX_NUMBER_OF_PERIPHERALS];
  boolean rts[MAX_NUMBER_OF_PERIPHERALS];

  void init(StorageUnit *storage);
  void gatherPeripherals();

  String getUpdatesFromServer();
  void updatePeripherals(String input);
  void updateLocalIPToServer();

  void start();

private:
  void registerPeripheral();
  void verifyAndRegisterPeripheral(String dev_id, int index, int re_attempt_times);
  void sendDevPerListToServer(String url, String peripheralsList);
  void sendPeripheralsListToServer();
  void sendTriggersListToServer();
  void printWiFiStatus();
  bool isIPChanged();
  String ipToString(IPAddress ip);
  boolean isNewPeripheralConnected(int index);
  void selectLine(int index);
  boolean checkForRTS();
};

#endif
