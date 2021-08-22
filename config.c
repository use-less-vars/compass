//PIC libs
#include <libpic30.h>
#include <xc.h>

//Standard libs
#include <stdio.h>
#include <string.h>
#include <math.h>

//my libs
#include "config.h"
#include "UART.h"

//defaults
#define CONFIG_DEFAULT_CYCLE_TIME           100
#define CONFIG_DEFAULT_NUMBER_OF_SAMPLES    1
#define CONFIG_DEFAULT_SUPPLY_VOLTAGE       5.0
#define CONFIG_DEFAULT_REF_VOLTAGE          0.0

//limits
#define CONFIG_MIN_CYCLE_TIME               10
#define CONFIG_MAX_SAMPLES                  20
#define CONFIG_MAX_SUPPLY_VOLTAGE           12.0
#define CONFIG_MAX_REF_VOLTAGE              10.0
#define CONFIG_MIN_REF_VOLTAGE              -10.0

//
#define CONFIG_PRINT_MODE_VERBOSE           0
#define CONFIG_PRINT_MODE_FAST              1

//local functions
void _config_eval_user_input(uint8_t* input_str);

//local vars
config_t config;



void config_init(){
    //there will be loading from permanent memory, but for now, we use standard vals
    config.cycle_time = CONFIG_DEFAULT_CYCLE_TIME;
    config.flipping_on = true;
    config.number_of_samples = CONFIG_DEFAULT_NUMBER_OF_SAMPLES;
    config.print_mode = CONFIG_PRINT_MODE_VERBOSE;
    config.ref_x = CONFIG_DEFAULT_REF_VOLTAGE;
    config.ref_y = CONFIG_DEFAULT_REF_VOLTAGE;
    config.ref_z = CONFIG_DEFAULT_REF_VOLTAGE;
    config.supply_x = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    config.supply_y = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    config.supply_z = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    //if a new full message has arrived, let UART call this function to evaluate it
    UART_register_callback_on_msg(_config_eval_user_input);
}

//logic to evaluate inputs from user and put them to the config
void _config_eval_user_input(uint8_t* input_str){
    //...
    printf("Some config came: %s\r\n", input_str);
    
}

uint8_t config_get_number_of_samples(){
    return config.number_of_samples;
} 

uint16_t config_get_cycle_time(){
    return config.cycle_time;
}

uint8_t config_get_print_mode(){
    return config.print_mode;
}

bool config_get_flipping_on(){
    return config.flipping_on;
}

double config_get_supply(uint8_t channel){
    switch(channel){
        case 0:
            return config.supply_x;
        break;
        case 1:
            return config.supply_y;
        break;
        case 2:
            return config.supply_z;
        break;
        default:
            return 0.0;
        break;
    }   
}

double config_get_ref(uint8_t channel){
    switch(channel){
        case 0:
            return config.ref_x;
        break;
        case 1:
            return config.ref_y;
        break;
        case 2:
            return config.ref_z;
        break;
        default:
            return 0.0;
        break;
    }   
}