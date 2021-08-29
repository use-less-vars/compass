//PIC libs
#include <libpic30.h>
#include <xc.h>

//Standard libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//my libs
#include "config.h"
#include "UART.h"
#include "DAC.h"

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

#define CONFIG_NUM_OF_ARGS                  5

//local functions
void _config_eval_user_input(uint8_t* input_str);
double _add_to_voltage(char* arg);

//local vars
config_t config;



void config_init(){
    //there will be loading from permanent memory, but for now, we use standard vals
    //load_from_memory();
    //
    config.cycle_time = CONFIG_DEFAULT_CYCLE_TIME;
    config.flipping_on = true;
    config.number_of_samples = CONFIG_DEFAULT_NUMBER_OF_SAMPLES;
    config.print_mode = CONFIG_PRINT_MODE_VERBOSE;
    config.ref_x = CONFIG_DEFAULT_REF_VOLTAGE;
    config.ref_y = -1.03;//vCONFIG_DEFAULT_REF_VOLTAGE;
    config.ref_z = CONFIG_DEFAULT_REF_VOLTAGE;
    config.supply_x = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    config.supply_y = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    config.supply_z = CONFIG_DEFAULT_SUPPLY_VOLTAGE;
    //if a new full message has arrived, let UART call this function to evaluate it
    UART_register_callback_on_msg(_config_eval_user_input);

    //set DAC-vals, supply voltage and reference voltage
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_supply_set(config_get_supply(i), i); 
    }
    for(uint8_t i = 0; i < 3; i++){
       DAC_v_ref_set(config_get_ref(i), i); 
    }
}

//logic to evaluate inputs from user and put them to the config
void _config_eval_user_input(uint8_t* input_str){
    
    printf("Some config came: %s\r\n", input_str);
    //input commands are separated by spaces only
    char delim[] = " \r\n";
    char* args[CONFIG_NUM_OF_ARGS];
    char* cmd;
    cmd = strtok((char*)input_str,delim);
    for(uint8_t i = 0; i < CONFIG_NUM_OF_ARGS; i++){
        args[i] = strtok(NULL, delim);
    }
    printf("CMD: %s, ARGS: %s %s %s %s %s\r\n", cmd, args[0],args[1],args[2],args[3],args[4]);
    
    //do specific configs, this can be expanded for more config commands to come
    
    //update ref voltage
    bool allowed = true;
    if(strcmp(cmd,"refx") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        allowed = allowed && config.ref_x+add_voltage <= CONFIG_MAX_REF_VOLTAGE;
        allowed = allowed && config.ref_x+add_voltage >= CONFIG_MIN_REF_VOLTAGE;
        config.ref_x += allowed ? add_voltage : 0;
        DAC_v_ref_set(config.ref_x, 0);
    }
    if(strcmp(cmd,"refy") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        allowed = allowed && config.ref_y+add_voltage <= CONFIG_MAX_REF_VOLTAGE;
        allowed = allowed && config.ref_y+add_voltage >= CONFIG_MIN_REF_VOLTAGE;
        config.ref_y += allowed ? add_voltage : 0;
        DAC_v_ref_set(config.ref_y, 1);
    }
    if(strcmp(cmd,"refz") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        allowed = allowed && config.ref_z+add_voltage <= CONFIG_MAX_REF_VOLTAGE;
        allowed = allowed && config.ref_z+add_voltage >= CONFIG_MIN_REF_VOLTAGE;
        config.ref_z += allowed ? add_voltage : 0;
        DAC_v_ref_set(config.ref_z, 2);
    }
    
    //update supply voltage
    if(strcmp(cmd,"supx") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        config.supply_x += config.supply_x+add_voltage <= CONFIG_MAX_SUPPLY_VOLTAGE ? add_voltage : 0;
        DAC_v_supply_set(config.supply_x, 0);
    }
    if(strcmp(cmd,"supy") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        config.supply_y += config.supply_y+add_voltage <= CONFIG_MAX_SUPPLY_VOLTAGE ? add_voltage : 0;
        DAC_v_supply_set(config.supply_y, 1);
    }
    if(strcmp(cmd,"supz") == 0){
        double add_voltage = _add_to_voltage(args[0]);
        config.supply_z += config.supply_z+add_voltage <= CONFIG_MAX_SUPPLY_VOLTAGE ? add_voltage : 0;
        DAC_v_supply_set(config.supply_z, 2);
    }
    
    if(strcmp(cmd,"average") == 0){
        char* ptr;
        uint8_t number_of_samples = strtol(args[0],&ptr,10);
        allowed = number_of_samples >= 1 && number_of_samples <= CONFIG_MAX_SAMPLES ? true : false;
        config.number_of_samples = allowed ? number_of_samples : config.number_of_samples;
    }
    
    if(strcmp(cmd,"save") == 0){
        //do save to flash
    }
    
    if(strcmp(cmd,"freq") == 0){
        char* ptr;
        uint16_t freq = strtol(args[0],&ptr,10);
        //convert frequency to times in ms
        allowed = 1000/freq >= CONFIG_MIN_CYCLE_TIME ? true : false;
        config.cycle_time = allowed ? 1000/freq : config.cycle_time;
    }
    if(strcmp(cmd,"flip") == 0){
        if(strcmp(args[0],"on") == 0){
            config.flipping_on = true;
        }else if(strcmp(args[0],"off") == 0){
            config.flipping_on = false;
        }else{
            //do nothing when wrong command
        }
    }
    
    if(strcmp(cmd,"print") == 0){
        if(strcmp(args[0],"verbose") == 0){
            config.print_mode = CONFIG_PRINT_MODE_VERBOSE;
        }else if(strcmp(args[0],"fast") == 0){
            config.print_mode = CONFIG_PRINT_MODE_FAST;
        }else{
            //maybe there will be more variants of printing data
        }
    }
    
    
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

double _add_to_voltage(char* arg){
    if(strcmp(arg,"+") == 0){
        return 0.01;
    }
    if(strcmp(arg,"++") == 0){
        return 0.1;
    }
    if(strcmp(arg,"+++") == 0){
        return 1.0;
    }
    if(strcmp(arg,"-") == 0){
        return -0.01;
    }
    if(strcmp(arg,"--") == 0){
        return -0.1;
    }
    if(strcmp(arg,"---") == 0){
        return -1.0;
    }
    //if no match is found, the to-be-added-voltage is 0
    return 0;
    
}