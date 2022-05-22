#include "LCDBoilerplate.h"

//Cunstroctur
LCDBoilerplate::LCDBoilerplate(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows): lcd(lcd_Addr, lcd_cols, lcd_rows) {
  this->lcd_Addr;
  this->lcd_cols;
  this->lcd_rows;
  lcd.begin(lcd_cols, lcd_rows);
  lcd.init();
  lcd.backlight();
}

void LCDBoilerplate::TurnOnBacklight(bool isOn) {
  if (!isOn) {
    lcd.noBacklight();
    return;
  }
  lcd.backlight();
  return;
}

void LCDBoilerplate::WriteFailedConnectWifi() {
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

void LCDBoilerplate::WriteConnectingWlan() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WLAN");
}

void LCDBoilerplate::WriteMQTTState(bool isSuccess) {
  if (!isSuccess) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Failed connect");
    lcd.setCursor(0, 1);
    lcd.print("MQTT Broker");
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Success connect");
    lcd.setCursor(0, 1);
    lcd.print("MQTT Broker");
    delay(2000);
  }
}

void LCDBoilerplate::Clear() {
  lcd.clear();
}
