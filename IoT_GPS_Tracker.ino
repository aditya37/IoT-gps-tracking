#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// PIN
#define LED_WARNING_PIN 2

// Consturct
LiquidCrystal_I2C lcd(0x27, 16, 2);
ESP8266WebServer httpServer(80);


// Constant
const char *wlanSSID = "Terrace Cafe 2";
const char *wlanPassword = "susujahe";

String htmlStrListNetwork = "";
// <li> Tag html
String htmlliTagNetwork = "";

// TODO: Lcd Boilerplate
void writeConnectionStateWlan(String text) {
  lcd.setCursor(2, 0);
  lcd.print(text);
}
void writeFailedToConnectWlan() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Failed to");
  lcd.setCursor(1, 1);
  lcd.print("connect wlan");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please open WLAN");
  lcd.setCursor(0, 1);
  lcd.print("Config Page");
}

/*
   EEPROM Manager
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
   Wlan manager
*/

// TODO: Create header for wifi manager
bool wlanConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return true;
}
void changeWlanModeToAccessPoint() {
  // Set mode to accessPoint
  WiFi.mode(WIFI_STA);
  // disconnect if connected with another wlan
  WiFi.disconnect();
  // break before scan a network
  delay(1000);

  // count available nearby network
  int countNearbyNet = WiFi.scanNetworks();
  Serial.print(countNearbyNet);
  Serial.print(" Nearby network");
  Serial.println("");

  // validate count nearby network
  if (countNearbyNet == 0) {
    Serial.println("Nearby network Not found");
  } else {

    // show detail network
    for (int net = 0; net < countNearbyNet; net++) {
      htmlliTagNetwork += "<li>" + WiFi.SSID(net) + "</li>";
    }
    Serial.println(htmlStrListNetwork);
  }
  // set SSID name
  // for connect to nodemcu and config wlan
  WiFi.softAP("IoT-Tracker-WLAN", "");
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
  // Init a lcd
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  // Init serial
  Serial.begin(9600);
  // Init LED
  pinMode(LED_WARNING_PIN, OUTPUT);
  // connect to last connected wifi
  WiFi.begin(wlanSSID, wlanPassword);
  httpServer.begin();

  // loop 5 times
  for (int count = 0; count <= 15; count++) {
    // blink led until connected wifi or failed to connect wifi
    digitalWrite(LED_WARNING_PIN, HIGH);

    if (!wlanConnected()) {
      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("Connecting.....");
      // if count > 9 will break and not connected to wifi
      if (count >= 5 ) {
        // lcd
        writeFailedToConnectWlan();
        // change mode to ap and serve http
        changeWlanModeToAccessPoint();
        // serve http
        webServerController();
        return;
      }
    } else {
      lcd.clear();
      digitalWrite(LED_WARNING_PIN, HIGH);
      writeConnectionStateWlan("Connected....");
      return;
    }
    digitalWrite(LED_WARNING_PIN, LOW);
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  httpServer.handleClient();
}
