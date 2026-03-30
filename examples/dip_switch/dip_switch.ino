/*
 * Project: MC145158 Arduino Library
 * File: dip_switch.ino
 * Version: 1.0.0.13
 * Date: 2026/03/30
 * License: GNU GPL v3.0
 * Developed by: Junon M. (2008-2026)
 * Description: Embedded systems and RF signal processing.
 */

#include "MC145158.h"

//---------------------------------------------------
//const int32_t INT_FREQ_KHz = -10700; // KHz (-10.7MHz) 
const int32_t INT_FREQ_KHz =      0; // KHz (0) 
//const int32_t INT_FREQ_KHz =  10700; // KHz (10.7MHz) 

const uint32_t PRESCALER = 8; // LB3500 (8)
const uint32_t CRYSTAL_KHz = 1000; // KHz (1MHz)
const uint32_t PHASE_DET_FREQ_Hz = 12500; // Hz (12500Hz)
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

  // Set DipSwitch Arduino Pins 
  pll.setDipSwitchPins(7,6,5,4,3,2,1,0);
 
  pll.config( 
  /* Intermediate Freq in KHz */ INT_FREQ_KHz, 
  /* External Prescaler Divider */ PRESCALER, 
  /* Ref Clock in KHz */ CRYSTAL_KHz, 
  /* Phase Det Freq in Hz */ PHASE_DET_FREQ_Hz);

  pll.setFrequencyFromDipSwitch();
}


void loop() {
}
