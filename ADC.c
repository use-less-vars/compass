//generated libs (MCC))
#include "pin_manager.h"
#include "system.h"
#include "spi1.h"
#include "interrupt_manager.h"
#include "tmr1.h"

//my libs
#include <limits.h>
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

//define states for the data acquisition
#define SPI_START_ACQUISITION                0
#define SPI_ACQUIRE_DATA        1
#define SPI_FINISHED_ACQUISITION    2

//define SPI data
typedef struct{
    uint8_t state;
    int32_t val_x;
    int32_t val_y;
    int32_t val_z;
    uint8_t sample_count_current;
    uint8_t sample_count_target;
}SPI_data_t;

//local functions
void _set_sample_rate();
void _get_adc_val();

//local vars
SPI_data_t spi;
uint8_t dataTransmitted[3*6];
uint8_t dataReceived[3*6];

void ADC_init(){
    IO_RA0_SetInterruptHandler(_get_adc_val); 
    _set_sample_rate();
    //go to "idle" state until application triggers acquisition
    spi.state = SPI_FINISHED_ACQUISITION;
    
    
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
// RX-Message from SPI contains Data, structured in 24Bit words:
// - Word 0: Status of the Device 
// - Word 1-4: 24Bit ADC-val of the n-th input
// - Word 5: CRC checksum (which, of course, is not used)

void _get_adc_val(){
    //CS Pin low enables the chip
    IO_RC9_SetLow();
    memset(dataTransmitted, 0, sizeof(dataTransmitted));
    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
    switch(spi.state){
        case SPI_START_ACQUISITION:
            //if the state is set to START_ACQUISITION, the first data sample is thrown away
            // for code simplicity
            spi.sample_count_current = 0;
            spi.val_x = 0;
            spi.val_y = 0;
            spi.val_z = 0;
            spi.state = SPI_ACQUIRE_DATA;
            break;
        case SPI_ACQUIRE_DATA:
            if(spi.sample_count_current < spi.sample_count_target){
                spi.sample_count_current++;
                //truncate to 16Bit (throw 8LSB away)
                
                //signedness should be correct
                spi.val_x += ((int16_t)dataReceived[3] << 8) | dataReceived[4];
                spi.val_y += ((int16_t)dataReceived[6] << 8) | dataReceived[7];
                spi.val_z += ((int16_t)dataReceived[9] << 8) | dataReceived[10];
            }else{
                spi.state = SPI_FINISHED_ACQUISITION;
            }
            break;
        case SPI_FINISHED_ACQUISITION:
            //Keep data and do nothing until the STM is reset externally.
            break;
        default:
            break;
    }
    IO_RC9_SetHigh();
}

void ADC_start_sampling(uint8_t number_of_samples){
    spi.sample_count_target = number_of_samples;
    spi.state = SPI_START_ACQUISITION;
}

bool ADC_has_finished(){
    return spi.state == SPI_FINISHED_ACQUISITION;
}
//make sure to pass a large enough array
void ADC_get_data(int16_t *data){
    data[0] = (int16_t)(spi.val_x/spi.sample_count_target);
    data[1] = (int16_t)(spi.val_y/spi.sample_count_target);
    data[2] = (int16_t)(spi.val_z/spi.sample_count_target);
}