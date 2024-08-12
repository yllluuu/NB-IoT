/*
 * gpio_i2c.h
 *
 *  Created on: 2024年6月14日
 *      Author: 杨璐
 */

#ifndef INC_GPIO_I2C_H_
#define INC_GPIO_I2C_H_

#include <stdio.h>

enum
{
	NO_ERROR		= 0x00,//no error
	PARM_ERROR  	= 0x01,//parameter out of rang
	ACK_ERROR		= 0x02,//no acknowledgment error
	CHECKSUM_ERROR	= 0x04,//checksum mismatch error
	TIMEOUT_ERROR	= 0x08,//timeout error
	BUS_ERROR		= 0x10,//bus busy
};

enum
{
	ACK_NONE,
	ACK,
	NAK,
};

extern int I2C_Master_Receive(uint8_t addr, uint8_t *buf, int len);
extern int I2C_Master_Transmit(uint8_t addr, uint8_t *data, int bytes);
#endif /* INC_GPIO_I2C_H_ */
