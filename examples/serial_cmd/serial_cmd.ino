/*
  Lib: PLL MC145158
  Version: 1.0.0.9
  Date: 2026/03/21
  Author: Junon M
  Hardware: Arduino Uno or Nano by Serial Monitor
*/

#include "MC145158.h"

const char * VERSION = "1.0.0.9";

//----------------------------------------------------------------
// Configuração do menu serial
//----------------------------------------------------------------
const char MENU_TEXT_FREQ[] = "Frequency";
const char INDEX_FREQ[] = "1";
const float MAX_FREQ = 150000.f;
const float MIN_FREQ = 1000.f;

const char MENU_TEXT_INT_FREQ[] = "Intermediate Frequency";
const char INDEX_INT_FREQ[] = "2";
const float MAX_INT_FREQ = 10700.f;
const float MIN_INT_FREQ = -10700.f;

const char MENU_TEXT_PRESCALER[] = "Prescaler";
const char INDEX_PRESCALER[] = "3";
const float MAX_PRESCALER = 512.0f;
const float MIN_PRESCALER = 0.0f;

const char MENU_TEXT_CRYSTAL[] = "Xtal";
const char INDEX_CRYSTAL[] = "4";
const float MAX_CRYSTAL = 30000.0f;
const float MIN_CRYSTAL = 500.0f;

const char MENU_TEXT_PHASE_DET_FREQ[] = "Phase Det Freq";
const char INDEX_PHASE_DET_FREQ[] = "5";
const float MAX_PHASE_DET_FREQ = 30000.f;
const float MIN_PHASE_DET_FREQ = 0.f;

const int SEP_COUNT = 41;
//----------------------------------------------------------------

//----------------------------------------------------------------
// Parâmetros iniciais
//----------------------------------------------------------------
float Frequency_KHz = 98500.f; // 98.5 MHz
float Intermediate_Frequency_KHz = 0.0f; // 0.0 MHz
float Prescaler = 8.f; // Ex: LB3500 (8)
float Crystal_KHz = 4000.f; // KHz (4MHz)
float Phase_Det_Freq_Hz = 1250.f; // Hz (for 10Khz steps)
//----------------------------------------------------------------

//----------------------------------------------------------------
// MC145158 pins to Arduino
//----------------------------------------------------------------
const int CLOCK_PIN = 25;//10;
const int DATA_PIN  = 26;//11;
const int LE_PIN  = 33;//12;
//----------------------------------------------------------------


MC145158 pll; // Declaração do objeto pll


void setup() {

  Serial.begin(115200);
  delay(100);   
  // Arduino pins
  pll.begin(CLOCK_PIN, DATA_PIN, LE_PIN);
  commitConfig();
  delay(1000);
  getCommands();
}


void loop() {
  commandInterpreter();
}


void commitConfig()
{
  pll.config( 
  /* Intermediate Freq in KHz */ (int32_t)Intermediate_Frequency_KHz, 
  /* External Prescaler Divider */ (uint32_t)Prescaler, 
  /* Ref Clock in KHz */ (uint32_t)Crystal_KHz, 
  /* Phase Det Freq in Hz */ (uint32_t)Phase_Det_Freq_Hz);

  pll.setFrequency(/* Freq in KHz */ (uint32_t)Frequency_KHz);
}


String Separator(int pos, int len, char ch)
{
  char corners = ' ';
  if (pos == 0) corners = ' ';
  else if (pos == 1) corners = '.';
  else if (pos == 2) corners = '|';
  else if (pos == 3) corners = '\'';

  String s = "\n" + String(corners);
  for (int i = 0; i < len; i++) s += ch;
  s += String(corners) + "\n";
  return s; 
}



String getCommands() 
{
  String msg = "\n";
  msg += Separator(1, SEP_COUNT, '=');
  msg += "|                   MENU                  |";
  msg += Separator(2, SEP_COUNT, '=');
  msg += " PLL MC145158 v" + String(VERSION) + "\n";
  msg += " Please choose an option:\n ";
  msg += String(INDEX_FREQ) + ". " + String(MENU_TEXT_FREQ) + " = " + String(Frequency_KHz, 0)  + "KHz\n ";
  msg += String(INDEX_INT_FREQ) + ". " + String(MENU_TEXT_INT_FREQ) + " = " + String(Intermediate_Frequency_KHz, 0)  + "KHz\n ";
  msg += String(INDEX_PRESCALER) + ". " + String(MENU_TEXT_PRESCALER) + " = " + String(Prescaler, 0)  + "\n ";
  msg += String(INDEX_CRYSTAL) + ". " + String(MENU_TEXT_CRYSTAL) + " = " + String(Crystal_KHz, 0)  + "KHz\n ";
  msg += String(INDEX_PHASE_DET_FREQ) + ". " + String(MENU_TEXT_PHASE_DET_FREQ) + " = " + String(Phase_Det_Freq_Hz, 0)  + "Hz";
// Recebe o ponteiro para o buffer estático
  const char* report = pll.getTuningStatus();
  msg += report;
  return msg;
}



void changeParam(String &returned_text, const String menu_label, const String menu_index, String text, float &value, const float min_value, const float max_value,  const String unit)
{
  static int pos = 0;
  String S = "";

  if (pos == 0)
  {
    returned_text = menu_index;
    S += Separator(0, SEP_COUNT, '-');
    S += "Enter " + menu_label + " (" + String(min_value, 2) + unit + " - " + String(max_value, 2) + unit + " )";
    Serial.println(S);
    pos++;
  }
  else
  {
    float number = text.toFloat();

    if ((number >= min_value) && (number <= max_value))
    {
      value = number;
      pos = 0;
      returned_text = "";
      S += "Response = " + String(number, 2) + unit;
      commitConfig();
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
      changeParam(lastS, MENU_TEXT_FREQ, INDEX_FREQ, text, Frequency_KHz, MIN_FREQ, MAX_FREQ, "KHz");
    }
    else if (S.equalsIgnoreCase(INDEX_INT_FREQ))
    {
      changeParam(lastS, MENU_TEXT_INT_FREQ, INDEX_INT_FREQ, text, Intermediate_Frequency_KHz, MIN_INT_FREQ, MAX_INT_FREQ, "KHz");
    }
    else if (S.equalsIgnoreCase(INDEX_PRESCALER))
    {
      changeParam(lastS, MENU_TEXT_PRESCALER, INDEX_PRESCALER, text, Prescaler, MIN_PRESCALER, MAX_PRESCALER, "");
    }
    else if (S.equalsIgnoreCase(INDEX_CRYSTAL))
    {
      changeParam(lastS, MENU_TEXT_CRYSTAL, INDEX_CRYSTAL, text, Crystal_KHz, MIN_CRYSTAL, MAX_CRYSTAL, "KHz");
    }
    else if (S.equalsIgnoreCase(INDEX_PHASE_DET_FREQ))
    {
      changeParam(lastS, MENU_TEXT_PHASE_DET_FREQ, INDEX_PHASE_DET_FREQ, text, Phase_Det_Freq_Hz, MIN_PHASE_DET_FREQ, MAX_PHASE_DET_FREQ, "Hz");
    }
    else
    {
      commitConfig();
      Serial.println(getCommands());
    }
  }
}
