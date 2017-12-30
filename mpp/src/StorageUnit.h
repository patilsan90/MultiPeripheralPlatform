#ifndef __STORAGE_UNIT__
#define __STORAGE_UNIT__
#include <string.h>
#include <Arduino.h>

typedef enum OPERATION_MODE {
  e_CONFIG_MODE,
  e_RECEPTION_MODE,
} OPERATION_MODE;

class StorageUnit
{
public:
  String wifi_ssid;
  String wifi_psw;
  String owner_id;
  String server_url;
  String local_ip = "NO_IP";

  String get_updates_url = "/device/get_updates";
  String update_peripheral_list_url = "/device/update_peripherals_list";
  String register_peripheral_url = "/device/register_peripheral";
  String update_ip_url = "/device/update_ip";
  String send_trigger_url = "/device/received_trigger_from_peripherals";

  int saveConfiguration(String input);
  void loadConfiguration();
  bool saveIP();
  OPERATION_MODE getOperarionMode();
  String getPlatformDeviceId();

private:
  String getOwner();
  void init();
  int convertStringToObj(String input);
  String ipToString(IPAddress ip);
};

#endif
