/*
 * File:   main.c
 * Author: Jojo
 *
 * Created on 15. Juni 2020, 19:45
 */


//generated libs (MCC))
#include "pin_manager.h"
#include "clock.h"
#include "system.h"
#include "spi1.h"
#include "uart1.h"
#include "i2c2.h"
#include "i2c1.h"
#include "interrupt_manager.h"
#include "tmr1.h"

//my libs
#include "DAC.h"
#include "project.h"
#include "timer.h"

//PIC libs
#include <libpic30.h>
#include <xc.h>

//Standard libs
#include <stdio.h>
#include <string.h>
#include <math.h>



#define SLAVE_I2C_GENERIC_RETRY_MAX           100
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT      50   // define slave timeout 

typedef struct{
    uint16_t x_val;
    uint16_t y_val;
    uint16_t z_val;
}data_point_t;


uint8_t dataTransmitted[3*6];
uint8_t dataReceived[3*6];
bool ADC_newDataAvailable;
data_point_t n_val;
data_point_t p_val;







//callback when new data available
void get_adc_val(){
    IO_RC9_SetLow();
    memset(dataTransmitted, 0, sizeof(dataTransmitted));
    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
    ADC_newDataAvailable = true;
    IO_RC9_SetHigh();
}

#define RX_BUFFER_SIZE  20
char rx_input[RX_BUFFER_SIZE];
bool do_print = 0;
uint8_t rx_index;
//the _U1RXInterrupt does something similar, but it seems to be bullshit.
void UART1_Receive_CallBack(){
    while((U1STAbits.URXDA == 1))
    {
        if(rx_index == 0){
            memset(rx_input,0,RX_BUFFER_SIZE);
        }
        rx_input[rx_index] = U1RXREG;;
        if(rx_index >= RX_BUFFER_SIZE-1){
            memset(rx_input,0,RX_BUFFER_SIZE);
            rx_index = 0;
        }else if(rx_input[rx_index] == '\r'){
            do_print = true;
            rx_index = 0;
        }else{
           rx_index++; 
        }  
    }
    
}

int main(void) {
    memset(rx_input,0,10);
    SYSTEM_Initialize();
    // INTERRUPT_GlobalEnable();
    //PIN_MANAGER_Initialize();
    CLOCK_Initialize();
    //INTERRUPT_Initialize();
    INTERRUPT_GlobalEnable();
    TMR1_SetInterruptHandler(timer_int_func);
    //UART1_SetRxInterruptHandler(uart_rx_callback);
    IO_RA0_SetInterruptHandler(get_adc_val); 
    I2C1_Initialize();
    I2C2_Initialize();
    uint16_t val = 0;
    
    memset(dataTransmitted,0,sizeof(dataTransmitted));
    //set OSR to 4096 (= 1kSPS)
    dataTransmitted[0] = 0b01100001;
    dataTransmitted[1] = 0b10000000;
    dataTransmitted[2] = 0;
    dataTransmitted[3] = 0b00000111;
    dataTransmitted[4] = 0b00010110;
    IO_RC9_SetLow();
    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
    IO_RC9_SetHigh();
    memset(dataTransmitted,0,sizeof(dataTransmitted));
//    //set Chop mode on
//    memset(dataTransmitted,0,sizeof(dataTransmitted));
//    dataTransmitted[0] = 0b01100011;
//    dataTransmitted[1] = 0b00000000;
//    dataTransmitted[2] = 0;
//    dataTransmitted[3] = 0b00000111;
//    dataTransmitted[4] = 0b00000000;
//    IO_RC9_SetLow();
//    SPI1_Exchange8bitBuffer(dataTransmitted, 3*6,dataReceived);
//    IO_RC9_SetHigh();
    LATAbits.LATA1 = 1;
    memset(dataTransmitted,0,sizeof(dataTransmitted));
    uint16_t val2 = 0;
    uint8_t val3 = 0;
    v_supply_set(7, 0);
    v_supply_set(7, 1);
    v_supply_set(7,2);
    v_ref_set(0, 0);
    v_ref_set(-0.25, 1);
    v_ref_set(0, 2);
    float v = -2.0;
    while(1){
        
        //switch()
        uint16_t val_x = (uint16_t)dataReceived[3]<<8;
        val_x += (uint16_t)dataReceived[4];
        //val += (uint32_t)dataReceived[5];
        //val = val << 8;
        uint16_t val_y = (uint16_t)dataReceived[6]<<8;
        val_y += (uint16_t)dataReceived[7];
        uint16_t val_z = (uint16_t)dataReceived[9]<<8;
        val_z += (uint16_t)dataReceived[10];
        
            
        printf("Val: %6d %6d %6d  %f\r\n", val_x, val_y, val_z,v);
        if(do_print){
            do_print = false;
            printf(" This was send: %s\r\n",rx_input);
        }
        
        __delay_ms(500); 
        v+= 0.01;
        v_ref_set(v, 1);
        v = v>= 2.0 ? -2.0:v;  
        
       
        
        val2 = val2 % 1480;
        if(val3 == 0){
            val2++;
        }
        val3++;
        val3 = val3%30;
       
    }
    
  
  
  return 0;
}
