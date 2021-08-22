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
#include "UART.h"
#include "project.h"
#include "timer.h"
#include "config.h"

//PIC libs
#include <libpic30.h>
#include <xc.h>

//Standard libs
#include <stdio.h>
#include <string.h>
#include <math.h>




int main(void) {
    SYSTEM_Initialize();
    INTERRUPT_GlobalEnable();
    timer_init();
    ADC_init();
    DAC_init();
    UART_init();
    config_init();
    uint16_t val = 0;
    
    uint16_t val2 = 0;
    uint8_t val3 = 0;
    //set DAC-vals. This will be replaced by values, loaded from storage
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_supply_set(config_get_supply(i), i); 
    }
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_ref_set(config_get_ref(i), i); 
    }
    timer_t *t1 = timer_create();
    timer_start_countdown(t1,1000);
    while(1){
        UART_update();
        
        if(timer_has_finished(t1)){
            printf("sec\r\n");
            timer_start_countdown(t1,1000);
        }
        
        
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
