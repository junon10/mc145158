/*
 Project: MC145158 Arduino Library
 File: MC145158.cpp
 Version: 1.0.0.14
 Date: 2026/04/24
 License: GNU GPL v3.0
 Developed by: Junon M. (2008-2026)
 Description: Embedded systems and RF signal processing.
*/

#include "MC145158.h"

MC145158::MC145158() {

  // Default config for dip switch pins (0 to 7)
  for (int i = 0; i < SW_COUNT; i++) _sw_pins[i] = i;

  _dip_sw_value = 0;

  _freq_KHz = 100000; // KHz (100MHz)
  _freq_shift_KHz = 0; // KHz (0 or 10700 or -10700)
  _prescaler = 8; // Ex: [div by 8 LB3500] (1)
  _xtal_KHz = 1000; // KHz (1MHz)
  _phase_det_freq_Hz = 12500; // Hz (12500Hz)
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


void MC145158::setDipSwitchPins(const uint8_t b7, const uint8_t b6, const uint8_t b5, const uint8_t b4, 
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


void MC145158::setFrequencyFromDipSwitch()
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


void MC145158::formatLine(char* dest, const char* label, const char* value, int width) {
    const int LABEL_WIDTH = 20; // Ajusta onde o ':' vai ficar
    dest[0] = '|';
    
    // 1. Adiciona o Label (ex: "Step Size")
    int labelLen = strlen(label);
    dest[1] = ' '; // Espaço inicial
    memcpy(dest + 2, label, labelLen);
    
    // 2. Preenche com espaços até o separador ':'
    for (int i = 2 + labelLen; i < LABEL_WIDTH; i++) {
        dest[i] = ' ';
    }
    
    // 3. Adiciona o separador e o valor
    dest[LABEL_WIDTH] = ':';
    dest[LABEL_WIDTH + 1] = ' ';
    int valLen = strlen(value);
    memcpy(dest + LABEL_WIDTH + 2, value, valLen);
    
    // 4. Preenche o resto da linha até a borda direita
    for (int i = LABEL_WIDTH + 2 + valLen; i < width - 1; i++) {
        dest[i] = ' ';
    }
    
    dest[width - 1] = '|';
    dest[width] = '\0';
}


void MC145158::centerText(char* buffer, const char* text, int width) {
    int len = strlen(text);
    if (len >= width - 2) {
        // Se o texto for maior que a largura, apenas copia o que couber
        strncpy(buffer + 1, text, width - 2);
        return;
    }

    int spaces = (width - 2 - len) / 2;
    int extra = (width - 2 - len) % 2; // Para lidar com textos de comprimento ímpar

    // Preenche com espaços iniciais
    for (int i = 1; i <= spaces; i++) buffer[i] = ' ';
    
    // Copia o texto
    memcpy(buffer + 1 + spaces, text, len);
    
    // Preenche com espaços finais
    for (int i = 1 + spaces + len; i < width - 1; i++) buffer[i] = ' ';
    
    // Garante as bordas e o terminador
    buffer[0] = '|';
    buffer[width - 1] = '|';
    buffer[width] = '\0';
}


const char* MC145158::getSeparator(int pos, int len, char ch) {
    // Buffer estático para a linha (ajuste o tamanho se len for > 60)
    static char sepBuffer[64];
    
    char corner;
    switch (pos) {
        case 1:  corner = '.';  break; // Topo
        case 2:  corner = '|';  break; // Meio / Divisória
        case 3:  corner = '\''; break; // Base
        default: corner = ' ';  break; // Espaço simples
    }

    // Limpa o buffer
    memset(sepBuffer, 0, sizeof(sepBuffer));

    int idx = 0;
    // Adiciona quebra de linha inicial se não for a primeira linha do log
    // sepBuffer[idx++] = '\r'; // Opcional, dependendo do seu terminal
    // sepBuffer[idx++] = '\n'; 

    sepBuffer[idx++] = corner;
    for (int i = 0; i < len; i++) {
        if (idx < (int)sizeof(sepBuffer) - 2) {
            sepBuffer[idx++] = ch;
        }
    }
    sepBuffer[idx++] = corner;
    sepBuffer[idx++] = '\0'; // Fim da string

    return sepBuffer;
}


const char* MC145158::getTuningStatus() {
    static char finalReport[1024]; 
    char line[128];
    char valBuf[64];
    const int SEP_WIDTH = 43;

    finalReport[0] = '\0';

    // Cabeçalho
    strcat(finalReport, getSeparator(1, SEP_WIDTH - 2, '='));
    strcat(finalReport, "\r\n");
    centerText(line, "MC145158 - HARDWARE REPORT", SEP_WIDTH);
    strcat(finalReport, line);
    strcat(finalReport, "\r\n");
    strcat(finalReport, getSeparator(2, SEP_WIDTH - 2, '='));
    strcat(finalReport, "\r\n");

    // --- SEÇÃO DE SISTEMA ---
    snprintf(valBuf, sizeof(valBuf), "%.2f kHz", (float)_real_step_Hz / 1000.0);
    formatLine(line, "Step Size", valBuf, SEP_WIDTH);
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    snprintf(valBuf, sizeof(valBuf), "%.3f MHz", (float)(_freq_KHz + _freq_shift_KHz) / 1000.0);
    formatLine(line, "Target", valBuf, SEP_WIDTH);
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    // --- LÓGICA DE STATUS E ERRO ---
    uint64_t totalHz = (uint64_t)(_freq_KHz + _freq_shift_KHz) * 1000ULL;
    if (totalHz % _real_step_Hz != 0) {
        float lockAtMHz = (float)((totalHz / _real_step_Hz) * _real_step_Hz) / 1000000.0;
        snprintf(valBuf, sizeof(valBuf), "%.4f MHz", lockAtMHz);
        formatLine(line, "Lock At", valBuf, SEP_WIDTH);
        strcat(finalReport, line); strcat(finalReport, "\r\n");

        formatLine(line, "Status", "!!! FREQ ERROR !!!", SEP_WIDTH);
    } else {
        formatLine(line, "Status", "PHASE LOCK READY", SEP_WIDTH);
    }
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    // --- SEÇÃO DE HARDWARE (REGISTRADORES) ---
    strcat(finalReport, "|-----------------------------------------|\r\n");

    snprintf(valBuf, sizeof(valBuf), "%u (Ref)", _r_final);
    formatLine(line, "R-Counter", valBuf, SEP_WIDTH);
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    snprintf(valBuf, sizeof(valBuf), "%u (MSB)", _n_final);
    formatLine(line, "N-Counter", valBuf, SEP_WIDTH);
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    snprintf(valBuf, sizeof(valBuf), "%u (LSB)", _a_final);
    formatLine(line, "A-Counter", valBuf, SEP_WIDTH);
    strcat(finalReport, line); strcat(finalReport, "\r\n");

    // Base
    strcat(finalReport, getSeparator(3, SEP_WIDTH - 2, '='));

    return finalReport;
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


