/*
 * eeprom.h
 *
 *  Created on: Oct 17, 2014
 *      Author: Clay, Jonathan
 */

#ifndef EEPROM_H_
#define EEPROM_H_
#include "DSP28x_Project.h"


void I2CA_Init(void);
Uint16 I2CA_WriteData(struct I2CMSG *msg);
Uint16 I2CA_ReadData(struct I2CMSG *msg);


#endif /* EEPROM_H_ */
