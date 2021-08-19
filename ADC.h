/* 
 * File:   ADC.h
 * Author: use-less-vars
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


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */

