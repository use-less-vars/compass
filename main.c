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


#define MAIN_STATE_P_FLIP           0
#define MAIN_STATE_P_WAIT           1
#define MAIN_STATE_P_MEAS           2
#define MAIN_STATE_N_FLIP           3
#define MAIN_STATE_N_WAIT           4
#define MAIN_STATE_N_MEAS           5
#define MAIN_STATE_EVAL             6
#define MAIN_STATE_SHOW             7
#define MAIN_STATE_UPDATE_CONFIG    8

uint8_t state;
int16_t p_val_x;
int16_t p_val_y;
int16_t p_val_z;
int16_t n_val_x;
int16_t n_val_y;
int16_t n_val_z;


int main(void) {
    int16_t data_buf[3];
    //init modules
    SYSTEM_Initialize();
    INTERRUPT_GlobalEnable();
    timer_init();
    ADC_init();
    DAC_init();
    UART_init();
    config_init();
    //set DAC-vals, supply voltage and reference voltage
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_supply_set(config_get_supply(i), i); 
    }
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_ref_set(config_get_ref(i), i); 
    }
    state = MAIN_STATE_P_FLIP;
    timer_t *t1 = timer_create();
    
    while(1){
        UART_update();
        if(state == MAIN_STATE_SHOW){
            printf("        x       y       z\r\n");
            printf("P: %6d, %6d, %6d\r\n", p_val_x,p_val_y,p_val_z);
            printf("N: %6d, %6d, %6d\r\n", n_val_x,n_val_y,n_val_z);
            printf("   %6d, %6d, %6d\r\n", p_val_x-n_val_x,p_val_y-n_val_y,p_val_z-n_val_z);
        }
        if(config_get_flipping_on()){
            switch(state){
                case MAIN_STATE_P_FLIP:
                    timer_start_countdown(t1,1);
                    P_FLIP_SetHigh();
                    state = MAIN_STATE_P_WAIT;
                    break;
                case MAIN_STATE_P_WAIT:
                    if(timer_has_finished(t1)){
                        P_FLIP_SetLow();
                        ADC_start_sampling(config_get_number_of_samples());
                        state = MAIN_STATE_P_MEAS;
                    }
                    break;
                case MAIN_STATE_P_MEAS:
                    if(ADC_has_finished()){
                        ADC_get_data(data_buf);
                        p_val_x = data_buf[0];
                        p_val_y = data_buf[1];
                        p_val_z = data_buf[2];
                        state = MAIN_STATE_N_FLIP;
                    }
                    break;
                case MAIN_STATE_N_FLIP:
                    timer_start_countdown(t1,1);
                    N_FLIP_SetHigh();
                    state = MAIN_STATE_N_WAIT;
                    break;
                case MAIN_STATE_N_WAIT:
                    if(timer_has_finished(t1)){
                        N_FLIP_SetLow();
                        ADC_start_sampling(config_get_number_of_samples());
                        state = MAIN_STATE_N_MEAS;
                    }
                    break;
                case MAIN_STATE_N_MEAS:
                    if(ADC_has_finished()){
                        ADC_get_data(data_buf);
                        n_val_x = data_buf[0];
                        n_val_y = data_buf[1];
                        n_val_z = data_buf[2];
                        state = MAIN_STATE_EVAL;
                    }
                    break;
                case MAIN_STATE_EVAL:
                    //do eval stuff here
                    state = MAIN_STATE_SHOW;
                    break;
                case MAIN_STATE_SHOW:
                    state = MAIN_STATE_P_FLIP;
                    break;
                default:
                    break;
            }
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
