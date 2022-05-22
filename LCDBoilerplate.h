#ifndef LCDBoilerplate_H
#define LCDBoilerplate_H
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LCDBoilerplate {
  private:
    LiquidCrystal_I2C lcd;
    uint8_t lcd_Addr;
    uint8_t lcd_cols;
    uint8_t lcd_rows;
  public:
    LCDBoilerplate(uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows);
    void TurnOnBacklight(bool isOn);
    void WriteFailedConnectWifi();
    void WriteConnectingWlan();
    void WriteMQTTState(bool isSuccess);
    void Clear();
};
#endif
