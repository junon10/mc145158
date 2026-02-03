/*
  Lib: PLL MC145158
  Version: 1.0.3
  Date: 2025/02/07
  Author: Junon M
  License: GPLv3
*/

#include "MC145158.h"

#define DEBUG   1
//#define EXAMPLE_DATA  1 

MC145158::MC145158() {

  // Default config for dip switch pins (0 to 7)
  for (int i = 0; i < SW_NUM_BITS; i++) _sw_pins[i] = i;

  _dip_sw_value = 0;

  _freq = 10.0f; // MHz (10MHz)
  _freq_shift = 0.0f; // MHz (0)
  _prescaler = 1.0f; // Ex: [div by 8 LB3500] (1)
  _xtal = 10000.0f; // KHz (10MHz)
  _phase_det_freq = 0.5f; // KHz (500Hz)
}


MC145158::~MC145158() {

  digitalWrite(_le_pin, LOW);
  digitalWrite(_clock_pin, LOW);
  digitalWrite(_data_pin, LOW);

  pinMode(_clock_pin, INPUT);
  pinMode(_data_pin, INPUT);
  pinMode(_le_pin, INPUT);
}


void MC145158::begin(const uint8_t clock_pin, const uint8_t data_pin, const uint8_t le_pin) {

  _clock_pin  = clock_pin;
  _data_pin  = data_pin;
  _le_pin = le_pin;

  pinMode(_clock_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
  pinMode(_le_pin, OUTPUT);

  digitalWrite(_le_pin, LOW);
  digitalWrite(_clock_pin, LOW);
  digitalWrite(_data_pin, LOW);
}


void MC145158::setDipSwPinout(const uint8_t b7, const uint8_t b6, const uint8_t b5, const uint8_t b4, 
const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0) 
{
_sw_pins[7] = b7;
_sw_pins[6] = b6;
_sw_pins[5] = b5;
_sw_pins[4] = b4;
_sw_pins[3] = b3;
_sw_pins[2] = b2;
_sw_pins[1] = b1;
_sw_pins[0] = b0;

for (int i = 0; i < SW_NUM_BITS; i++) pinMode(_sw_pins[i], INPUT_PULLUP);

}


void MC145158::bitDelay()
{
  delay(1);
}


void MC145158::sendData(uint32_t data, uint8_t len)
{
  uint32_t D = data;
   
  for (int i = len - 1; i >= 0; i--) 
  {
    // Verifica o bit i deslocando e comparando
    bool bit = (D >> i) & 1;
    
    digitalWrite(_data_pin, bit ? HIGH : LOW);
    
#ifdef DEBUG
    Serial.print(bit ? "1" : "0");
#endif   

    MC145158::bitDelay();
    digitalWrite (_clock_pin, HIGH); // rising edge latches data
    MC145158::bitDelay();
    digitalWrite (_clock_pin, LOW);
  }

  digitalWrite (_data_pin, HIGH);

#ifdef DEBUG
  Serial.print(F(", HEX="));
  Serial.print(D, HEX);
  Serial.print(F(", DEC="));
  Serial.print(D);
  Serial.println();
#endif

}


void MC145158::pulseLe(void)
{
  digitalWrite (_le_pin, HIGH);
  MC145158::bitDelay();
  digitalWrite (_le_pin, LOW);
}


void MC145158::config(float f_shift_MHz, float prescaler, float xtal_KHz, float phase_det_freq_KHz)
{
  _freq_shift = f_shift_MHz;
  _prescaler = prescaler;
  _xtal = xtal_KHz;
  _phase_det_freq = phase_det_freq_KHz;
}


void MC145158::setFrequency(float MHz)
{
  _freq = MHz;
  MC145158::commitConfig();
}


void MC145158::setFreqShift(float MHz)
{
  _freq_shift = MHz;
  MC145158::commitConfig();
}


void MC145158::setFrequencyByDipSw()
{
  for (int i = 0; i < SW_NUM_BITS; i++)
  {
    _dip_sw_value = bitWrite(_dip_sw_value, i, !digitalRead(_sw_pins[i])); 
  }

  // freq = 1080 - 255; // = 825
  float freq = FACTOR - _dip_sw_value;
  freq /= 10; // = 82,5 MHz;
  MC145158::setFrequency(freq);
}


void MC145158::commitConfig()
{
  _R_counter = (uint16_t)(_xtal / _phase_det_freq); 

  // Cálculo do N total (divisor total)
  // f_vco = [(P * N) + A] * f_ref
  float f_vco = (_freq + _freq_shift) * 1000.0f; // KHz
  uint32_t total_divider = (uint32_t)(f_vco / _phase_det_freq);

  // Separação para síntese dual-modulus:
  _N_counter = total_divider / _prescaler;
  _A_counter = total_divider % _prescaler;

#ifdef DEBUG
   Serial.println();
   Serial.println(F("Calculado:"));
   Serial.print(F("A+N=")); Serial.println(_d);
   Serial.print(F("A=0x")); Serial.print(_A_counter, HEX); Serial.print(F(", A=")); Serial.print(_A_counter);
   Serial.print(F(", N=0x")); Serial.print(_N_counter, HEX); Serial.print(F(", N=")); Serial.println(_N_counter);
   Serial.print(F("R=")); Serial.println(_R_counter);
   Serial.println();
#endif

  // --- Envio de N e A (18 bits total: 10 de N, 7 de A, 1 de Controle) ---
  // Estrutura: [10 bits N] [7 bits A] [Bit C=0]
  uint32_t dataNA = 0;
  dataNA |= (_N_counter & 0x3FF) << 8; // Desloca N para o topo
  dataNA |= (_A_counter & 0x7F) << 1;  // A no meio
  dataNA |= 0;                         // Bit de controle C=0 (LSB)


#ifdef DEBUG
  Serial.println(F("Conteúdo dos Registradores:"));
  Serial.print("N+A = ");
#endif
  MC145158::sendData(dataNA, 18);
  MC145158::pulseLe();

  // --- Envio de R (15 bits total: 14 de R, 1 de Controle) ---
  // Estrutura: [14 bits R] [Bit C=1]
  uint16_t dataR = 0;
  dataR |= (_R_counter & 0x3FFF) << 1; // 14 bits de R
  dataR |= 1;                          // Bit de controle C=1 (LSB)

#ifdef DEBUG
  Serial.print("R   =    ");
#endif
  MC145158::sendData(dataR, 15);
  MC145158::pulseLe();
}


