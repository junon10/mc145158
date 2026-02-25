/*
  Lib: PLL MC145158
  Version: 1.0.0.8
  Date: 2026/02/25
  Author: Junon M
  License: GPLv3
*/

#ifndef MC145158_h
#define MC145158_h

#include <Arduino.h>

//==============================================================================
//                              Constantes
//==============================================================================
#define FACTOR        1080
#define SW_NUM_BITS      8
//==============================================================================


class MC145158
{
  private:
    uint32_t _A_counter, _N_counter, _R_counter;
    uint8_t _clock_pin, _data_pin, _le_pin;
    uint8_t _dip_sw_value;
    uint8_t _sw_pins[8];
    float _freq, _freq_shift, _prescaler, _xtal, _phase_det_freq;
    void bitDelay();
    void sendData(uint32_t data, uint8_t len);
    void pulseLe();

  public:

    MC145158();

    virtual ~MC145158();
    
    void begin(const uint8_t clock_pin, const uint8_t data_pin, const uint8_t le_pin);

    void setDipSwPinout(const uint8_t b7, const uint8_t b6, const uint8_t b5, const uint8_t b4, 
    const uint8_t b3, const uint8_t b2, const uint8_t b1, const uint8_t b0); 

    void config(float f_shift_MHz = 0.0f, float prescaler = 1.0f, float xtal_KHz = 4000.0f, float phase_det_freq_KHz = 0.5f);
    
    void setFrequency(float MHz);

    // Freq_Shift = 10.7    // Receptor FM (F + 10,7MHz)
    // Freq_Shift = 0           // Transmissor FM
    // Freq_Shift = -10.7   // Receptor FM (F - 10,7MHz)
    //
    void setFreqShift(float MHz);

    void setFrequencyByDipSw();

    void commitConfig();
};

#endif /* MC145158_h */
