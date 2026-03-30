/*
 * Project: MC145158 Arduino Library
 * File: display.ino
 * Version: v1.0.0.12
 * Date: 2026/03/29
 * License: GNU GPL v3.0
 * Developed by: Junon M. (2008-2026)
 * Description: Embedded systems and RF signal processing.
 */

#include <LiquidCrystal.h>
#include "MC145158.h"

//---------------------------------------------------------
uint32_t FREQUENCY_KHz = 98500;            // KHz (98.5MHz)

//const int32_t INT_FREQ_KHz = -10700;     // KHz (-10.7MHz) 
const int32_t INT_FREQ_KHz =        0;     // KHz     (0MHz) 
//const int32_t INT_FREQ_KHz =  10700;     // KHz  (10.7MHz) 

const uint32_t PRESCALER = 8;              // LB3500 1/8
const uint32_t CRYSTAL_KHz = 4000;         // KHz (4MHz)
const uint32_t PHASE_DET_FREQ_Hz = 12500;  // Hz  (12500Hz)
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
  /* Intermediate Freq in KHz */ INT_FREQ_KHz, 
  /* External Prescaler Divider */ PRESCALER, 
  /* Ref Clock in KHz */ CRYSTAL_KHz, 
  /* Phase Det Freq in KHz */ PHASE_DET_FREQ_Hz);

  pll.setFrequency(/* Freq in KHz */ FREQUENCY_KHz);

  Serial.println();
  Serial.println("PLL MC145158");
  Serial.println();

  lcd.setCursor(0,0);
  lcd.print("PLL MC145158");
  
  lcd.setCursor(0,1);
  lcd.print("Freq: ");
  lcd.print(FREQUENCY_KHz, 0);
  lcd.print("KHz");
}



void loop() { 
}
