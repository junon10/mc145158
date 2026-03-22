/*
  Lib: PLL MC145158
  Version: 1.0.0.10
  Date: 2026/03/22
  Author: Junon M
  License: GPLv3
*/

#ifndef MC145158_h
#define MC145158_h

#include <Arduino.h>

//==============================================================================
//                              Constants
//==============================================================================
#define FACTOR        1080
#define SW_COUNT      8
//==============================================================================

typedef union RegisterNA {
    struct {
        uint32_t control : 1;  // Bit C (deve ser 0 para N+A)
        uint32_t a_cnt   : 7;  // Swallow Counter (A) - 7 bits
        uint32_t n_cnt   : 10; // Programmable Counter (N) - 10 bits
        uint32_t padding : 14; // Completa os 32 bits do uint32_t
    } bits;
    uint32_t full_data;        // Acesso ao valor completo para envio
};


typedef union RegisterR {
    struct {
        uint16_t control : 1;  // Bit C (deve ser 1 para R)
        uint16_t r_cnt   : 14; // Reference Counter (R) - 14 bits
        uint16_t padding : 1;  // Completa os 16 bits do uint16_t
    } bits;
    uint16_t full_data;        // Acesso ao valor completo para envio
};


class MC145158
{
  private:
    uint32_t _A_counter, _N_counter, _R_counter;
    uint8_t _clock_pin, _data_pin, _le_pin;
    uint8_t _dip_sw_value;
    uint8_t _sw_pins[8];
    int32_t _freq_KHz, _freq_shift_KHz;
    uint32_t _freq_total_KHz;
    uint32_t _prescaler, _xtal_KHz, _phase_det_freq_Hz, _real_step_Hz, _r_calc;
    uint16_t _r_final, _n_final;
    uint8_t _a_final;
    void bitDelay();
    void sendData(uint32_t data, uint8_t len);
    void pulseLe();

  public:
    MC145158();
    virtual ~MC145158();
    void begin(const uint8_t clock_pin, const uint8_t data_pin, const uint8_t le_pin);
    void setDipSwitchPins(const uint8_t b7, const uint8_t b6, const uint8_t b5, const uint8_t b4, 
    const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0); 
    void config(int32_t f_shift_KHz = 0, uint32_t prescaler = 1, uint32_t xtal_KHz = 4000, uint32_t phase_det_freq_Hz = 500);
    void setFrequency(uint32_t KHz);
    void setFreqShift(int32_t KHz);
    void setFrequencyFromDipSw();
    void formatLine(char* dest, const char* label, const char* value, int width);
    void centerText(char* buffer, const char* text, int width);
    const char* getSeparator(int pos, int len, char ch);
    const char* getTuningStatus();
    void commitConfig();
};

#endif /* MC145158_h */
