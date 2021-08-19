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
#include "ADC.h"
#include "project.h"
#include "timer.h"

//PIC libs
#include <libpic30.h>
#include <xc.h>

//Standard libs
#include <stdio.h>
#include <string.h>
#include <math.h>


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
    timer_init();
    //UART1_SetRxInterruptHandler(uart_rx_callback);
    ADC_init();
    DAC_init();
    //I2C2_Initialize();
    uint16_t val = 0;
    
    uint16_t val2 = 0;
    uint8_t val3 = 0;
    //set DAC-vals. This will be replaced by values, loaded from storage
    DAC_v_supply_set(7, 0);
    DAC_v_supply_set(7, 1);
    DAC_v_supply_set(7,2);
    DAC_v_ref_set(0, 0);
    DAC_v_ref_set(-0.25, 1);
    DAC_v_ref_set(0, 2);
    float v = -2.0;
    while(1){
        
        //switch()
//        uint16_t val_x = (uint16_t)dataReceived[3]<<8;
//        val_x += (uint16_t)dataReceived[4];
//        //val += (uint32_t)dataReceived[5];
//        //val = val << 8;
//        uint16_t val_y = (uint16_t)dataReceived[6]<<8;
//        val_y += (uint16_t)dataReceived[7];
//        uint16_t val_z = (uint16_t)dataReceived[9]<<8;
//        val_z += (uint16_t)dataReceived[10];
//        
//            
//        printf("Val: %6d %6d %6d  %f\r\n", val_x, val_y, val_z,v);
//        if(do_print){
//            do_print = false;
//            printf(" This was send: %s\r\n",rx_input);
//        }
//        
//        __delay_ms(500); 
//        v+= 0.01;
//        v_ref_set(v, 1);
//        v = v>= 2.0 ? -2.0:v;  
//        
//       
//        
//        val2 = val2 % 1480;
//        if(val3 == 0){
//            val2++;
//        }
//        val3++;
//        val3 = val3%30;
//       
    }
    
  
  
  return 0;
}
