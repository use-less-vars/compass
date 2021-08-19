
//my libs
#include "project.h"

//PIC libs
#include <xc.h>
#include "DAC.h"
#include <libpic30.h>
#include <stdbool.h>

//generated libs
#include "i2c2.h"
#include "i2c1.h"

//std libs
#include <stdio.h>

//not used
#define SLAVE_I2C_GENERIC_RETRY_MAX           100
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT      50   // define slave timeout 


void DAC_init(){
    I2C1_Initialize();    
}

void DAC_v_ref_set(float voltage, uint8_t channel){
    uint16_t val = (uint16_t)((-voltage+(float)15.15)*151.7);
    printf("Ref Voltage: %f, val: %d, channel %d\r\n",voltage,val,channel);
    uint8_t temp = 0b01011000 | (channel << 1);
    printf("Channel: %0x\r\n", temp );
    I2C1CONLbits.SEN = 1;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    I2C1TRN = 0b11000000;
    uint8_t a = I2C1STAT;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    I2C1TRN = temp;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    I2C1TRN = val >> 8;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    I2C1TRN = val & 0xFF;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    I2C1CONLbits.PEN = 1;
    while(IFS1bits.MI2C1IF == 0);
    IFS1bits.MI2C1IF = 0;
    __delay_ms(50);
}

void DAC_v_supply_set(float voltage, uint8_t channel){
    uint16_t val = (uint16_t)(voltage*4096/(3.6786*3.3));
    printf("Supply Voltage: %f, val: %d, channel %d\r\n",voltage,val,channel);
    uint8_t temp = 0b01011000 | (channel << 1);
    printf("Channel: %0x\r\n", temp );
    I2C2CONLbits.SEN = 1;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = 0b11000000;
    uint8_t a = I2C2STAT;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = temp;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = val >> 8;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2TRN = val & 0xFF;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    I2C2CONLbits.PEN = 1;
    while(IFS3bits.MI2C2IF == 0);
    IFS3bits.MI2C2IF = 0;
    __delay_ms(50);
}

//SPI related functions

