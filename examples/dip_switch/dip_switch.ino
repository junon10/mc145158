/*
  Lib: PLL MC145158
  Version: 1.0.0.8
  Date: 2026/02/25
  Author: Junon M
  Hardware: Arduino Uno or Nano with DipSwitch
*/

#include "MC145158.h"

//---------------------------------------------------
const float INT_FREQ = 0.0f; // MHz (-10.7M, 0.0M, 10.7M) 
const float PRESCALER = 8.0f; // LB3500 (8)
const float XTAL = 4000.0f; // KHz (4MHz)
const float PHASE_DET_FREQ = 0.5f; // KHz (500Hz)
//---------------------------------------------------

//---------------------------------------------------
// MC145158 Arduino pins
//---------------------------------------------------
const int CLOCK_PIN = 10;
const int DATA_PIN  = 11;
const int LE_PIN  = 12;
//---------------------------------------------------


MC145158 pll;

void setup() {

  Serial.println();
  Serial.println("PLL MC145158");
  Serial.println();
   
  // Set PLL Arduino pinout
  pll.begin(CLOCK_PIN, DATA_PIN, LE_PIN);

  // Set DipSwitch Arduino pinout 
  pll.setDipSwPinout(7,6,5,4,3,2,1,0);
 
  pll.config( 
  /* Intermediate Freq in MHz */ INT_FREQ, 
  /* External Prescaler Divider */ PRESCALER, 
  /* Ref Clock in KHz */ XTAL, 
  /* Phase Det Freq in KHz */ PHASE_DET_FREQ);

  setFrequencyByDipSw();
}


void loop() {
}
