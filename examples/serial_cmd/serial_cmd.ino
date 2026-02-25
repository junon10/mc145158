/*
  Lib: PLL MC145158
  Version: 1.0.0.8
  Date: 2026/02/25
  Author: Junon M
  Hardware: Arduino Uno or Nano controlled by serial port
*/

#include "MC145158.h"

const char * VERSION = "1.0.1";

//----------------------------------------------------------------
// Configuração do menu serial
//----------------------------------------------------------------
const char MENU_TEXT_FREQ[] = "Frequency";
const char INDEX_FREQ[] = "1";
const float MAX_FREQ = 150.f;
const float MIN_FREQ = 0.5f;

const char MENU_TEXT_INT_FREQ[] = "Intermediate Frequency";
const char INDEX_INT_FREQ[] = "2";
const float MAX_INT_FREQ = 10.7f;
const float MIN_INT_FREQ = -10.7f;

const char MENU_TEXT_PRESCALER[] = "Prescaler";
const char INDEX_PRESCALER[] = "3";
const float MAX_PRESCALER = 512.0f;
const float MIN_PRESCALER = 0.0f;

const char MENU_TEXT_XTAL[] = "Xtal";
const char INDEX_XTAL[] = "4";
const float MAX_XTAL = 20000.0f;
const float MIN_XTAL = 500.0f;

const char MENU_TEXT_PHASE_DET_FREQ[] = "Phase Det Freq";
const char INDEX_PHASE_DET_FREQ[] = "5";
const float MAX_PHASE_DET_FREQ = 50.0f;
const float MIN_PHASE_DET_FREQ = 0.1f;

const int SEP_COUNT = 60;
//----------------------------------------------------------------

//----------------------------------------------------------------
// Parâmetros iniciais
//----------------------------------------------------------------
float Frequency = 10.0f; // 10.0 MHz
float Intermediate_Frequency = 0.0f; // 0.0 MHz
float Prescaler = 1.0f; // Ex: LB3500 (8)
float Xtal = 10000.0f; // KHz (10MHz)
float Phase_Det_Freq = 0.5f; // KHz (500Hz)
//----------------------------------------------------------------

//----------------------------------------------------------------
// MC145158 pins to Arduino
//----------------------------------------------------------------
const int CLOCK_PIN = 10;
const int DATA_PIN  = 11;
const int LE_PIN  = 12;
//----------------------------------------------------------------


MC145158 pll; // Declaração do objeto pll


void setup() {

  Serial.begin(115200);
   
  // Arduino pins
  pll.begin(CLOCK_PIN, DATA_PIN, LE_PIN);
  
  commitConfig();
}


void loop() {
  commandInterpreter();
}


void commitConfig()
{
  pll.config( 
  /* Intermediate Freq in MHz */ Intermediate_Frequency, 
  /* External Prescaler Divider */ Prescaler, 
  /* Ref Clock in KHz */ Xtal, 
  /* Phase Det Freq in KHz */ Phase_Det_Freq);

  pll.setFrequency(/* Freq in MHz */ Frequency);
}


String Separator(int len)
{
  String s = "\n";
  for (int i = 0; i < len; i++) s += "-";
  s += "\n";
  return s; 
}



String getCommands() 
{
  String msg = "";
  msg += Separator(SEP_COUNT);
  msg += "PLL MC145158\nVersion: " + String(VERSION) + "\n\n";
  msg += "Choose an option:\n\n";
  msg += String(INDEX_FREQ) + ". " + String(MENU_TEXT_FREQ) + " = " + String(Frequency, 3)  + "MHz\n";
  msg += String(INDEX_INT_FREQ) + ". " + String(MENU_TEXT_INT_FREQ) + " = " + String(Intermediate_Frequency, 3)  + "MHz\n";
  msg += String(INDEX_PRESCALER) + ". " + String(MENU_TEXT_PRESCALER) + " = " + String(Prescaler, 0)  + "\n";
  msg += String(INDEX_XTAL) + ". " + String(MENU_TEXT_XTAL) + " = " + String(Xtal, 2)  + "KHz\n";
  msg += String(INDEX_PHASE_DET_FREQ) + ". " + String(MENU_TEXT_PHASE_DET_FREQ) + " = " + String(Phase_Det_Freq, 2)  + "KHz\n";
  msg += Separator(SEP_COUNT);
  return msg;
}



void changeParam(String &returned_text, const String menu_label, const String menu_index, String text, float &value, const float min_value, const float max_value,  const String unit)
{
  static int pos = 0;
  String S = "";

  if (pos == 0)
  {
    returned_text = menu_index;
    S += Separator(SEP_COUNT);
    S += "Enter the value for " + menu_label + " between " + String(min_value, 2) + unit + " and " + String(max_value, 2) + unit;
    Serial.println(S);
    pos++;
  }
  else
  {
    float number = text.toFloat();

    if ((number >= min_value) && (number <= max_value))
    {
      value = number;
      commitConfig();
      pos = 0;
      returned_text = "";
      S += "Response = " + String(number, 2) + unit;
      S += getCommands();
      Serial.println(S);
    }
    else
    {
      S = "Error, the value " + String(number, 2) + unit + " is out of range, try again!";
      Serial.println(S);
    }
  }

}




void commandInterpreter()
{
  static String lastS = "";
  String S = "";

  if (Serial.available())
  {
    String text = Serial.readStringUntil('\n');

    S = text;

    if (!lastS.equals("")) S = lastS;

    if (S.equalsIgnoreCase(INDEX_FREQ))
    {
      changeParam(lastS, MENU_TEXT_FREQ, INDEX_FREQ, text, Frequency, MIN_FREQ, MAX_FREQ, "MHz");
    }
    else if (S.equalsIgnoreCase(INDEX_INT_FREQ))
    {
      changeParam(lastS, MENU_TEXT_INT_FREQ, INDEX_INT_FREQ, text, Intermediate_Frequency, MIN_INT_FREQ, MAX_INT_FREQ, "MHz");
    }
    else if (S.equalsIgnoreCase(INDEX_PRESCALER))
    {
      changeParam(lastS, MENU_TEXT_PRESCALER, INDEX_PRESCALER, text, Prescaler, MIN_PRESCALER, MAX_PRESCALER, "");
    }
    else if (S.equalsIgnoreCase(INDEX_XTAL))
    {
      changeParam(lastS, MENU_TEXT_XTAL, INDEX_XTAL, text, Xtal, MIN_XTAL, MAX_XTAL, "KHz");
    }
    else if (S.equalsIgnoreCase(INDEX_PHASE_DET_FREQ))
    {
      changeParam(lastS, MENU_TEXT_PHASE_DET_FREQ, INDEX_PHASE_DET_FREQ, text, Phase_Det_Freq, MIN_PHASE_DET_FREQ, MAX_PHASE_DET_FREQ, "KHz");
    }
    else
    {
      Serial.println(getCommands());
    }
  }
}
