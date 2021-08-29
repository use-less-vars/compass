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

typedef struct{
    uint8_t state;
    int16_t p_val_x;
    int16_t p_val_y;
    int16_t p_val_z;
    int16_t n_val_x;
    int16_t n_val_y;
    int16_t n_val_z;
    int32_t diff_x;
    int32_t diff_y;
    int32_t diff_z;
    int16_t offset_x;
    int16_t offset_y;
    int16_t offset_z;
}main_data_t;

main_data_t main_data;

int main(void) {
    int16_t data_buf[3];
    bool uart_empty = false;
    //init modules
    SYSTEM_Initialize();
    INTERRUPT_GlobalEnable();
    timer_init();
    ADC_init();
    DAC_init();
    UART_init();
    config_init();
    
    main_data.state = MAIN_STATE_P_FLIP;
    timer_t *t1 = timer_create();
    timer_t *t2 = timer_create();
    timer_start_countdown(t2,2000);
    while(1){
        uart_empty = UART_update();
        if(main_data.state == MAIN_STATE_SHOW){
            if(timer_has_finished(t2) && uart_empty){
                printf("        x       y       z\r\n");
                printf("P: %6d, %6d, %6d\r\n", main_data.p_val_x,main_data.p_val_y,main_data.p_val_z);
                printf("N: %6d, %6d, %6d\r\n", main_data.n_val_x,main_data.n_val_y,main_data.n_val_z);
                printf("   %6ld, %6ld, %6ld\r\n", main_data.diff_x,main_data.diff_y,main_data.diff_z);
                printf("O: %6d, %6d, %6d\r\n", main_data.offset_x, main_data.offset_y, main_data.offset_z);
                printf("angle: %f,\r\n", atan2(main_data.diff_x,main_data.diff_y)* 180 / 3.1415);
                timer_start_countdown(t2,200);
            }       
        }
        if(config_get_flipping_on()){
            switch(main_data.state){
                case MAIN_STATE_P_FLIP:
                    timer_start_countdown(t1,3);
                    P_FLIP_SetHigh();
                    main_data.state = MAIN_STATE_P_WAIT;
                    break;
                case MAIN_STATE_P_WAIT:
                    if(timer_has_finished(t1)){
                        P_FLIP_SetLow();
                        ADC_start_sampling(config_get_number_of_samples());
                        main_data.state = MAIN_STATE_P_MEAS;
                    }
                    break;
                case MAIN_STATE_P_MEAS:
                    if(ADC_has_finished()){
                        ADC_get_data(data_buf);
                        main_data.p_val_x = data_buf[0];
                        main_data.p_val_y = data_buf[1];
                        main_data.p_val_z = data_buf[2];
                        main_data.state = MAIN_STATE_N_FLIP;
                    }
                    break;
                case MAIN_STATE_N_FLIP:
                    timer_start_countdown(t1,3);
                    N_FLIP_SetHigh();
                    main_data.state = MAIN_STATE_N_WAIT;
                    break;
                case MAIN_STATE_N_WAIT:
                    if(timer_has_finished(t1)){
                        N_FLIP_SetLow();
                        ADC_start_sampling(config_get_number_of_samples());
                        main_data.state = MAIN_STATE_N_MEAS;
                    }
                    break;
                case MAIN_STATE_N_MEAS:
                    if(ADC_has_finished()){
                        ADC_get_data(data_buf);
                        main_data.n_val_x = data_buf[0];
                        main_data.n_val_y = data_buf[1];
                        main_data.n_val_z = data_buf[2];
                        main_data.state = MAIN_STATE_EVAL;
                    }
                    break;
                case MAIN_STATE_EVAL:
                    //do eval stuff here
                    main_data.diff_x = (int32_t)main_data.p_val_x-(int32_t)main_data.n_val_x;
                    main_data.diff_y = (int32_t)main_data.p_val_y-(int32_t)main_data.n_val_y;
                    main_data.diff_z = (int32_t)main_data.p_val_z-(int32_t)main_data.n_val_z;
                    main_data.offset_x = ((int32_t)main_data.p_val_x+(int32_t)main_data.n_val_x)/2;
                    main_data.offset_y = ((int32_t)main_data.p_val_y+(int32_t)main_data.n_val_y)/2;
                    main_data.offset_z = ((int32_t)main_data.p_val_z+(int32_t)main_data.n_val_z)/2;
                    main_data.state = MAIN_STATE_SHOW;
                    timer_start_countdown(t1,50);
                    break;
                case MAIN_STATE_SHOW:
                    if(timer_has_finished(t1)){
                        main_data.state = MAIN_STATE_P_FLIP;
                        timer_stop(t1);
                    }
                    
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
