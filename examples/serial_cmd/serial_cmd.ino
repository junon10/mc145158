/*
  Lib: PLL MC145158
  Version: 1.0.0.10
  Date: 2026/03/22
  Author: Junon M
  Hardware: Arduino Uno or Nano from Serial Monitor
*/

#include "MC145158.h"

const char * VERSION = "1.0.0.10";

//----------------------------------------------------------------
// Configuração do menu serial
//----------------------------------------------------------------
const char MENU_TEXT_FREQ[] = "VCO Freq";
const char INDEX_FREQ[] = "1";
const float MAX_FREQ = 150000.f;
const float MIN_FREQ = 1000.f;

const char MENU_TEXT_INT_FREQ[] = "IF";
const char INDEX_INT_FREQ[] = "2";
const float MAX_INT_FREQ = 10700.f;
const float MIN_INT_FREQ = -10700.f;

const char MENU_TEXT_PRESCALER[] = "Prescaler Freq";
const char INDEX_PRESCALER[] = "3";
const float MAX_PRESCALER = 512.0f;
const float MIN_PRESCALER = 0.0f;

const char MENU_TEXT_CRYSTAL[] = "Xtal Freq";
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
float Freq_KHz = 98500.f; // 98.5 MHz
float Int_Freq_KHz = 0.0f; // 0.0 MHz
float Prescaler = 8.f; // Ex: LB3500 (8)
float Crystal_KHz = 1000.f; // KHz (1MHz)
float Phase_Det_Freq_Hz = 12500.f; // Hz (for 100Khz steps)
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
  /* Intermediate Freq in KHz */ (int32_t)Int_Freq_KHz, 
  /* External Prescaler Divider */ (uint32_t)Prescaler, 
  /* Ref Clock in KHz */ (uint32_t)Crystal_KHz, 
  /* Phase Det Freq in Hz */ (uint32_t)Phase_Det_Freq_Hz);

  pll.setFrequency(/* Freq in KHz */ (uint32_t)Freq_KHz);
}


const char* getCommands() {
    static char msgBuffer[1024]; 
    char line[128]; 
    char labelBuf[64];
    char valBuf[64];
    const int SEP_WIDTH = 43; // Largura total (41 de preenchimento + 2 bordas)

    msgBuffer[0] = '\0';

    // 1. Topo e Título Centralizado
    strcat(msgBuffer, "\r\n");
    strcat(msgBuffer, pll.getSeparator(1, SEP_WIDTH - 2, '='));
    strcat(msgBuffer, "\r\n");
    
    pll.centerText(line, "PLL CONTROL MENU", SEP_WIDTH);
    strcat(msgBuffer, line);
    strcat(msgBuffer, "\r\n");

    // 2. Divisória e Versão
    strcat(msgBuffer, pll.getSeparator(2, SEP_WIDTH - 2, '='));
    strcat(msgBuffer, "\r\n");
    
    // Versão centralizada ou alinhada
    snprintf(valBuf, sizeof(valBuf), "v%s", VERSION);
    pll.formatLine(line, "Firmware", valBuf, SEP_WIDTH);
    strcat(msgBuffer, line);
    strcat(msgBuffer, "\r\n");
    
    strcat(msgBuffer, "| Please choose an option:                |\r\n");
    strcat(msgBuffer, "|-----------------------------------------|\r\n");

    // 3. Itens do Menu (Usando formatLine para alinhar os ':')
    
    // Item 1: Frequência
    snprintf(labelBuf, sizeof(labelBuf), "%s. %s", INDEX_FREQ, MENU_TEXT_FREQ);
    snprintf(valBuf, sizeof(valBuf), "%.0f KHz", Freq_KHz);
    pll.formatLine(line, labelBuf, valBuf, SEP_WIDTH);
    strcat(msgBuffer, line); strcat(msgBuffer, "\r\n");

    // Item 2: FI
    snprintf(labelBuf, sizeof(labelBuf), "%s. %s", INDEX_INT_FREQ, MENU_TEXT_INT_FREQ);
    snprintf(valBuf, sizeof(valBuf), "%.0f KHz", Int_Freq_KHz);
    pll.formatLine(line, labelBuf, valBuf, SEP_WIDTH);
    strcat(msgBuffer, line); strcat(msgBuffer, "\r\n");

    // Item 3: Prescaler
    snprintf(labelBuf, sizeof(labelBuf), "%s. %s", INDEX_PRESCALER, MENU_TEXT_PRESCALER);
    snprintf(valBuf, sizeof(valBuf), "%.0f", Prescaler);
    pll.formatLine(line, labelBuf, valBuf, SEP_WIDTH);
    strcat(msgBuffer, line); strcat(msgBuffer, "\r\n");

    // Item 4: Cristal
    snprintf(labelBuf, sizeof(labelBuf), "%s. %s", INDEX_CRYSTAL, MENU_TEXT_CRYSTAL);
    snprintf(valBuf, sizeof(valBuf), "%.0f KHz", Crystal_KHz);
    pll.formatLine(line, labelBuf, valBuf, SEP_WIDTH);
    strcat(msgBuffer, line); strcat(msgBuffer, "\r\n");

    // Item 5: Fase
    snprintf(labelBuf, sizeof(labelBuf), "%s. %s", INDEX_PHASE_DET_FREQ, MENU_TEXT_PHASE_DET_FREQ);
    snprintf(valBuf, sizeof(valBuf), "%.0f Hz", Phase_Det_Freq_Hz);
    pll.formatLine(line, labelBuf, valBuf, SEP_WIDTH);
    strcat(msgBuffer, line); strcat(msgBuffer, "\r\n");

    // 4. Fechamento do Menu
    strcat(msgBuffer, pll.getSeparator(3, SEP_WIDTH - 2, '='));
    strcat(msgBuffer, "\r\n");

    // 5. Anexar o Hardware Report (ele já vem com as próprias bordas)
    strcat(msgBuffer, pll.getTuningStatus());

    return msgBuffer;
}


void changeParam(String &returned_text, const String menu_label, const String menu_index, String text, float &value, const float min_value, const float max_value,  const String unit)
{
  static int pos = 0;
  String S = "";

  if (pos == 0)
  {
    returned_text = menu_index;
    S += "\r\nEnter New " + menu_label + " (" + String(min_value, 2) + unit + " - " + String(max_value, 2) + unit + " )";
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
      changeParam(lastS, MENU_TEXT_FREQ, INDEX_FREQ, text, Freq_KHz, MIN_FREQ, MAX_FREQ, "KHz");
    }
    else if (S.equalsIgnoreCase(INDEX_INT_FREQ))
    {
      changeParam(lastS, MENU_TEXT_INT_FREQ, INDEX_INT_FREQ, text, Int_Freq_KHz, MIN_INT_FREQ, MAX_INT_FREQ, "KHz");
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
