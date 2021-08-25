/* 
 * File:   ADC.h
 * Author: use-less-vars
 * 
 * Customized Lib to read data from the ADS131M04. This is highly customized and 
 * by no means reflects the full set of capabilities of this chip. 
 *
 * Created on 19. August 2021, 13:07
 */

#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <stdio.h> 
#include <stdint.h>
 
//global data types
    
typedef struct{
    uint16_t x_val;
    uint16_t y_val;
    uint16_t z_val;
}data_point_t;

void ADC_init();

void ADC_start_sampling(uint8_t number_of_samples);
bool ADC_has_finished();
void ADC_get_data(int16_t *data);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

