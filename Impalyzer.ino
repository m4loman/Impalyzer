/*********************

IMPALYZER
Impedance analyzer based on AD5933

Jan Zimmermann 2016
maloman@gmail.com

Part of the code is based on:
Adafruit RGB_LCD_Library
AD5933 Library from Analog Devices as well as code from Z_Logger

**********************/

// includes
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include "AD5933.h"

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Some definitions for the display and AD5933
#define WHITE 0x7
#define TWI_FREQ 400000L
#define cycles_base 200
#define cycles_multiplier 4
#define start_frequency 1000
#define cal_samples 30

const int numofIncrement = 1;
const double calResistance = 100000;
double gainFactor, pShift;

void setup() {
  // Debugging output
  Wire.begin();
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  lcd.print("IMPALYZER 0.1");
  lcd.setBacklight(WHITE);
  // set up AD5933
  AD5933.setExtClock(true);
  AD5933.resetAD5933();
  AD5933.setStartFreq(start_frequency);
  AD5933.setSettlingCycles(cycles_base, cycles_multiplier);
  AD5933.setStepSizeInHex(1);
  AD5933.setNumofIncrement(2);
  AD5933.setPGA(GAIN_1);
  AD5933.setRange(RANGE_1);

}
uint8_t sel_mode = 0;
uint8_t i=0;
void loop() {
  lcd.setCursor(0, 1);
  double temp = AD5933.getTemperature();
  AD5933.tempUpdate();

  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    
      if (buttons & BUTTON_UP) {
        lcd.print("AD5933 TEMP (C) ");
        lcd.setCursor(0,1);
        lcd.print(temp);
        lcd.print((char)223);
        sel_mode = 0;
      }
      
      if (buttons & BUTTON_DOWN) {
        AD5933.setCtrMode(REPEAT_FREQ);
        lcd.print("Impedance");
        lcd.setCursor(0,1);
        double cReal, cImag;
        double Z_Val, phase;
        AD5933.getComplex(gainFactor, pShift, Z_Val, phase);
        lcd.print(Z_Val/1000);
        lcd.print('k');
        lcd.print((char)244);
        lcd.print(" ");
        lcd.print(phase);
        lcd.print((char)223);
        sel_mode = 1;
      }
      
       if (buttons & BUTTON_SELECT & (sel_mode == 1)) {
        while (sel_mode == 1){
          AD5933.setCtrMode(REPEAT_FREQ);
          lcd.print("Impedance");
          lcd.setCursor(0,1);
          double cReal, cImag;
          double Z_Val, phase;
          AD5933.getComplex(gainFactor, pShift, Z_Val, phase);
          lcd.print(Z_Val/1000);
          lcd.print('k');
          lcd.print((char)244);
          lcd.print(" ");
          lcd.print(phase);
          lcd.print((char)223);
        }   
      }
    
      if (buttons & BUTTON_LEFT) {
        lcd.print("Cal with 100k");
        lcd.print((char)244);
        sel_mode = 2;
      }
      
       if (buttons & BUTTON_SELECT & (sel_mode == 2)) {
        AD5933.getGainFactor(calResistance, cal_samples, gainFactor, pShift, false);
        lcd.setCursor(0,0);
        lcd.print("Gain: ");
        lcd.print(gainFactor);
        lcd.setCursor(0,1);
        lcd.print("SpS: ");
        lcd.print(pShift);
        sel_mode = 1;       
      }
      
      if (buttons & BUTTON_RIGHT) {
        lcd.print("nothing here ");
        sel_mode = 0;
      }
}
}

