#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>

// custom header
#include "WLANManager.h"
#include "LCDBoilerplate.h"
#include "PUBSUBManager.h"

// PIN
#define LED_WARNING_PIN 2
#define SDAPIN 2 // D2
#define SCLPIN 16 // D1

// Constant
String DEVICE_ID = "iot.1bf7a743-d86a-4709-9832-3f522d4cf645";
const char* NOTIFY_TOPIC = "/device/notify/iot.1bf7a743-d86a-4709-9832-3f522d4cf645";
const char* HEARTBEAT_TOPIC = "/device/heartbeat/iot.1bf7a743-d86a-4709-9832-3f522d4cf645";
const char *mqtt_broker = "192.168.1.16";
const char *mqtt_username = "";
const char *mqtt_password = "";
int mqtt_port = 1883;
int HEARTBEAT_DURATION = 5000;
const char *wlanSSID = "KEDAI DJENGGOT $$$";
const char *wlanPassword = "tulislangsung";

//INSTANCE
ESP8266WebServer httpServer(80);
LCDBoilerplate lb(0x27, SDAPIN, SCLPIN);
WLANManager wl(lb);
PUBSUBManager pb(mqtt_broker, mqtt_username, mqtt_password, mqtt_port);

String htmlStrListNetwork = "";
// <li> Tag html
String htmlliTagNetwork = "";


/*
   EEPROM Manager (Memory)
*/
String getLastWlanPassword() {
  String strPassword = "";
  return strPassword;
}
String getLastWlanSSID() {
  String strSSID = "";
  return strSSID;
}
void clearEEPROM(int eepromRange) {
  for ( int i = 0; i < eepromRange; ++i) {
    // do clear
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

/*
  Http Template
*/
String htmlNearbyWlanList() {
  htmlStrListNetwork += "";
  htmlStrListNetwork += "<b> Nearby Wlan: </b>";
  htmlStrListNetwork += "<ul>";
  htmlStrListNetwork += htmlliTagNetwork;
  htmlStrListNetwork += "</ul>";
  htmlStrListNetwork += "<form action='/network/connect' >";
  htmlStrListNetwork += "SSID:<br> <input type='text' name='ssid'>";
  htmlStrListNetwork += "<br></br>";
  htmlStrListNetwork += "Password:<br> <input type='text' name='password'>";
  htmlStrListNetwork += "<br></br>";
  htmlStrListNetwork += "<br> <input type='submit'>";
  htmlStrListNetwork += "</form>";
  return htmlStrListNetwork;
}

/*
   Web Manager
*/
void healthCheckHandler() {
  httpServer.send(200, "text/html", "<b>UP</b>");
}
void connectToWlanHandler() {
  String ssid = httpServer.arg("ssid");
  String password = httpServer.arg("password");

  // clean last eeprom/wlan
  clearEEPROM(96);

  String back = "<a href='/network/nearby'>Back</a>";
  httpServer.send(200, "text/html", back);
}
void pageNearbyNetworkHandler() {
  String strHtml  = htmlNearbyWlanList();
  httpServer.send(200, "text/html", htmlStrListNetwork);
}
void webServerController() {
  httpServer.on("/", healthCheckHandler);
  httpServer.on("/network/nearby", pageNearbyNetworkHandler);
  httpServer.on("/network/connect", connectToWlanHandler);
}


void setup() {

  // Init serial
  Serial.begin(9600);
  //  TODO: remove
  httpServer.begin();

  // Init LED
  pinMode(LED_WARNING_PIN, OUTPUT);

  // connect to last connected wifi
  wl.ConnectToWlan(wlanSSID, wlanPassword);

  // wlan connection ticker
  bool tickWlanConn = wl.WlanConnectionTicker(40, 20);
  if (!tickWlanConn) {
    wl.AccessPointMode("IoT-Tracking-Device");
  } else {
    bool isConnectedBroker = pb.ConnectingToBroker(DEVICE_ID);
    if (!isConnectedBroker) {
      // write to lcd if not connected
      lb.WriteMQTTState(false);
    };
    // write to lcd if connected to broker
    lb.WriteMQTTState(true);
    // Message callback
    pb.MessageCallback();
    // publish notify
    //pb.Publish(NOTIFY_TOPIC, "tes");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  httpServer.handleClient();
  pb.Loop();
  pb.Heartbeat(HEARTBEAT_DURATION, HEARTBEAT_TOPIC, DEVICE_ID);
}
