/*
  Lib: PLL MC145158
  Version: 1.0.0.9
  Date: 2026/03/21
  Author: Junon M
  License: GPLv3
*/

#include "MC145158.h"

MC145158::MC145158() {

  // Default config for dip switch pins (0 to 7)
  for (int i = 0; i < SW_COUNT; i++) _sw_pins[i] = i;

  _dip_sw_value = 0;

  _freq_KHz = 10000; // KHz (10MHz)
  _freq_shift_KHz = 0; // KHz (0 or 10700 or -10700)
  _prescaler = 1; // Ex: [div by 8 LB3500] (1)
  _xtal_KHz = 10000; // KHz (10MHz)
  _phase_det_freq_Hz = 1250; // Hz (1250Hz)
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

for (int i = 0; i < SW_COUNT; i++) pinMode(_sw_pins[i], INPUT_PULLUP);

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
    //Serial.print(bit ? "1" : "0");
#endif   

    MC145158::bitDelay();
    digitalWrite (_clock_pin, HIGH); // rising edge latches data
    MC145158::bitDelay();
    digitalWrite (_clock_pin, LOW);
  }

  digitalWrite (_data_pin, HIGH);

#ifdef DEBUG
  //Serial.print(F(", HEX="));
  //Serial.print(D, HEX);
  //Serial.print(F(", DEC="));
  //Serial.print(D);
  //Serial.println();
#endif

}


void MC145158::pulseLe(void)
{
  digitalWrite (_le_pin, HIGH);
  MC145158::bitDelay();
  digitalWrite (_le_pin, LOW);
}


void MC145158::config(int32_t f_shift_KHz, uint32_t prescaler, uint32_t xtal_KHz, uint32_t phase_det_freq_Hz)
{
  _freq_shift_KHz = f_shift_KHz;
  _prescaler = prescaler;
  _xtal_KHz = xtal_KHz;
  _phase_det_freq_Hz = phase_det_freq_Hz;
}


void MC145158::setFrequency(uint32_t KHz)
{
  _freq_KHz = KHz;
  MC145158::commitConfig();
}


void MC145158::setFreqShift(int32_t KHz)
{
  _freq_shift_KHz = KHz;
  MC145158::commitConfig();
}


void MC145158::setFrequencyByDipSw()
{
  for (int i = 0; i < SW_COUNT; i++) {
    _dip_sw_value = bitWrite(_dip_sw_value, i, !digitalRead(_sw_pins[i])); 
  }
  // freq_KHz = 1080 - 255; -> 825
  // freq_KHz = 825 * 100; -> 82500
  int32_t freq_KHz = FACTOR - _dip_sw_value;
  freq_KHz *= 100; // = 82500 KHz;
  MC145158::setFrequency(freq_KHz);
}


const char* MC145158::getTuningStatus() {
    static char buffer[512]; 
    
    float stepKHz = (float)_real_step_Hz / 1000.0;
    float targetMHz = (float)_freq_total_KHz / 1000.0;
    uint32_t totalHz = (uint32_t)_freq_KHz * 1000UL;
    
    // Buffer temporário para a linha de status/erro dinâmico
    char statusLine[64];
    if (totalHz % _real_step_Hz != 0) {
        float lockAt = (float)((totalHz / _real_step_Hz) * _real_step_Hz) / 1000000.0;
        snprintf(statusLine, sizeof(statusLine), 
                 " [!] LOCK AT      : %.4f MHz\r\n [!] STATUS       : FREQ ERROR", lockAt);
    } else {
        snprintf(statusLine, sizeof(statusLine), 
                 " [+] STATUS       : PHASE LOCK READY");
    }

    // Formatação final do relatório
    // Usamos \r\n para compatibilidade total com monitores seriais (Windows/Linux)
    snprintf(buffer, sizeof(buffer),
        "\r\n|=========================================|\r\n"
        "|        MC145158 - HARDWARE REPORT       |\r\n"
        "|=========================================|\r\n"
        " [SYS] Step Size  : %.2f kHz\r\n"
        " [SYS] Target     : %.3f MHz\r\n"
        "%s\r\n"
        "|-----------------------------------------|\r\n"
        " [REG] R-Counter  : %u (Ref)\r\n"
        " [REG] N-Counter  : %u (MSB)\r\n"
        " [REG] A-Counter  : %u (LSB)\r\n"
        "'========================================='",
        stepKHz, targetMHz, statusLine, _r_final, _n_final, _a_final
    );

    return buffer; 
}


void MC145158::commitConfig() {

    RegisterNA regNA = {0};
    RegisterR  regR = {0};

    // 1. Cálculos de Base
    // Cálculo do Step Real (Ex: 1250Hz * 8 = 10000Hz)
    _real_step_Hz = _phase_det_freq_Hz * _prescaler;
    _freq_total_KHz = _freq_KHz + _freq_shift_KHz;
    _r_calc = ((uint32_t)_xtal_KHz * 1000UL) / _phase_det_freq_Hz;
    
    // Divisor que o chip "enxerga" após o LB3500
    uint32_t total_div_calc = ((uint32_t)_freq_total_KHz * 1000UL) / _phase_det_freq_Hz / _prescaler;

    // 2. Extração dos bits (Lógica de 17 bits)
    _r_final = constrain(_r_calc, 0, 16383);
    _n_final = (total_div_calc >> 7) & 0x3FF;
    _a_final = (total_div_calc & 0x7F);

    // 3. Montagem das Unions e Envio
    regR.bits.control = 1;
    regR.bits.r_cnt   = _r_final;
    
    regNA.bits.control = 0;
    regNA.bits.a_cnt   = _a_final;
    regNA.bits.n_cnt   = _n_final;

    MC145158::sendData(regNA.full_data, 18);
    MC145158::pulseLe();
    MC145158::sendData(regR.full_data, 15);
    MC145158::pulseLe();
}


