/* 
 * File:   config.h
 * Author: use-less-vars
 *
 * Created on 22. August 2021, 00:04
 */

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#ifndef CONFIG_H
#define	CONFIG_H

#define CONFIG_PRINT_MODE_VERBOSE           0
#define CONFIG_PRINT_MODE_FAST              1

typedef struct {
    uint8_t number_of_samples;
    uint16_t cycle_time;
    bool flipping_on;
    uint8_t print_mode;
    double ref_x;
    double ref_y;
    double ref_z;
    double supply_x;
    double supply_y;
    double supply_z;
}config_t;

void config_init();

uint8_t config_get_number_of_samples();
uint16_t config_get_cycle_time();
uint8_t config_get_print_mode();
bool config_get_flipping_on();
double config_get_supply(uint8_t channel);
double config_get_ref(uint8_t channel);


#endif	/* CONFIG_H */

