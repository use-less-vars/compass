/* 
 * File:   IO_setup.h
 * Author: use-less-vars
 *
 * Created on August 19, 2021, 12:14 AM
 */

#ifndef DAC_H
#define	DAC_H

#ifdef	__cplusplus
extern "C" {
#endif

void DAC_init(); 
void DAC_v_ref_set(double voltage, uint8_t channel);
void DAC_v_supply_set(double voltage, uint8_t channel);

#ifdef	__cplusplus
}
#endif

#endif	/* IO_H */

