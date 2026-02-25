/*
  Lib: PLL MC145158
  Version: 1.0.0.8
  Date: 2026/02/25
  Author: Junon M
  Hardware: Arduino Uno and Display module with keypad
*/

#include <LiquidCrystal.h>
#include "MC145158.h"

//---------------------------------------------------------
float FREQUENCY = 100.0f; // MHz (100MHz)
const float INT_FREQ = 0.0f; // MHz (-10.7M, 0.0M, 10.7M) 
const float PRESCALER = 8.0f; // LB3500 1/8
const float XTAL = 4000.0f; // KHz (4MHz)
const float PHASE_DET_FREQ = 0.5f; // KHz (500Hz)
//---------------------------------------------------------

//---------------------------------------------------------
// MC145158 pins to Arduino
//---------------------------------------------------------
const int CLOCK_PIN = 10;
const int DATA_PIN  = 11;
const int LE_PIN  = 12;
//---------------------------------------------------------

//---------------------------------------------------------
//LCD pin to Arduino
//---------------------------------------------------------
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7;
//---------------------------------------------------------

MC145158 pll;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);


void setup() {

  Serial.begin(115200);
  lcd.begin(16, 2);
   
  pll.begin(CLOCK_PIN, DATA_PIN, LE_PIN);

  pll.config( 
  /* Intermediate Freq in MHz */ INT_FREQ, 
  /* External Prescaler Divider */ PRESCALER, 
  /* Ref Clock in KHz */ XTAL, 
  /* Phase Det Freq in KHz */ PHASE_DET_FREQ);

  pll.setFrequency(/* Freq in MHz */ FREQUENCY);

  Serial.println();
  Serial.println("PLL MC145158");
  Serial.println();

  lcd.setCursor(0,0);
  lcd.print("PLL MC145158");
  
  lcd.setCursor(0,1);
  lcd.print("Freq: ");
  lcd.print(FREQUENCY, 1);
  lcd.print("MHz");
}



void loop() { 
}
