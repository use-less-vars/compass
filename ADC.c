//generated libs (MCC))
#include "pin_manager.h"
#include "system.h"
#include "spi1.h"
#include "interrupt_manager.h"
#include "tmr1.h"

//my libs
#include "ADC.h"
#include "DAC.h"
#include "project.h"
#include "timer.h"

//PIC libs
#include <libpic30.h>
#include <xc.h>

#include <string.h>
#include <stdio.h>
#include <stdint.h>

//local functions
void _set_sample_rate();
void _get_adc_val();

//local vars
uint8_t dataTransmitted[3*6];
uint8_t dataReceived[3*6];
bool newDataAvailable;
data_point_t n_val;
data_point_t p_val;

void ADC_init(){
    IO_RA0_SetInterruptHandler(_get_adc_val); 
    _set_sample_rate();
    
}

//only one mode is supported at the moment, see datasheet for OSR-settings
void _set_sample_rate(){
    memset(dataTransmitted,0,sizeof(dataTransmitted));
    //chop mode on
    //set OSR to 4096 (= 1kSPS)
    dataTransmitted[0] = 0b01100001;
    dataTransmitted[1] = 0b10000000;
    dataTransmitted[2] = 0;
    dataTransmitted[3] = 0b00000111;
    dataTransmitted[4] = 0b00010110;
    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
    
//    //set Chop mode on (not used at the moment)
//    memset(dataTransmitted,0,sizeof(dataTransmitted));
//    dataTransmitted[0] = 0b01100011;
//    dataTransmitted[1] = 0b00000000;
//    dataTransmitted[2] = 0;
//    dataTransmitted[3] = 0b00000111;
//    dataTransmitted[4] = 0b00000000;
//    IO_RC9_SetLow();
//    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
//    IO_RC9_SetHigh();
}

//callback when new data available
void _get_adc_val(){
    IO_RC9_SetLow();
    memset(dataTransmitted, 0, sizeof(dataTransmitted));
    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
    newDataAvailable = true;
    IO_RC9_SetHigh();
}