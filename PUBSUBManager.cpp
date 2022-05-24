#include "PUBSUBManager.h"
#include "WLANManager.h"

PUBSUBManager::PUBSUBManager(const char *mqtt_broker, const char *mqtt_username, const char *mqtt_password, int mqtt_port, WLANManager wlm, LCDBoilerplate lb): pubsub(espclient), lb(lcd_Addr, lcd_cols, lcd_rows), wlm(lb) {
  this->mqtt_broker = mqtt_broker;
  this->mqtt_username = mqtt_username;
  this->mqtt_password = mqtt_password;
  this->mqtt_port = mqtt_port;
  this->lb = lb;
  this->wlm = wlm;
  pubsub.setServer(mqtt_broker, mqtt_port);
}

bool PUBSUBManager::ConnectingToBroker(String device_id) {
  while (!pubsub.connected()) {
    if (pubsub.connect(device_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println(" mqtt broker connected");
      return true;
    } else {
      Serial.print("failed with state ");
      Serial.print(pubsub.state());
      return false;
    }
  }
}

void PUBSUBManager::MessageCallback(std::function<void(char*, uint8_t*, unsigned int)> callback) {
  pubsub.setCallback(callback);
}

//Publish....
bool PUBSUBManager::Publish(const char* topic, const char* payload) {
  bool isPublished = pubsub.publish(topic, payload);
  if (!isPublished) {
    return false;
  } else {
    return true;
  }
}
// Subscribe....
boolean PUBSUBManager::Subscribe(const char* topic, uint8_t qos) {
  bool subs = pubsub.subscribe(topic, qos);
  if (!subs) {
    return false;
  }
  return true;
}

// ==========================================================
// for device management in backend
// ==========================================================
bool PUBSUBManager::RequestRegisterDevice(const char* topic, String device_id, unsigned long timestamp) {
  StaticJsonDocument<1024> doc;
  doc["device_id"] = device_id;
  doc["mac_address"] = wlm.GetMacAddress();
  doc["device_type"] = "ESP8266";
  doc["chip_id"] = ESP.getChipId();
  doc["timestamp"] = timestamp;
  serializeJson(doc, RegisterDevicePayload);
  bool isPublished = Publish(topic, RegisterDevicePayload);
  if (!isPublished) {
    return false;
  }
  return true;
}

//Loop...
void PUBSUBManager::Loop() {
  pubsub.loop();
}
