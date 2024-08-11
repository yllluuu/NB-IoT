/********************************************************************************
 *      Copyright:  (C) 2024 yanglu 
 *                  All rights reserved.
 *
 *       Filename:  leds.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(17/07/24)
 *         Author:  yanglu <yanglu@2012174595@qq.com>
 *      ChangeLog:  1, Release initial version on "17/07/24 19:37:11"
 *                 
 ********************************************************************************/


#ifndef  _LEDS_H_
#define  _LEDS_H_


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <gpiod.h>

#include "gpiod.h"
#include "logger.h"
#define DELAY     300

#define ON        1
#define OFF       0

enum
{
	LED_R,
	LED_B,
	LED_G,
	LED_MAX,
};

enum
{
	ACTIVE_HIGH,
	ACTIVE_LOW,
};

typedef struct led_s
{
	const char                 *name;
	int                         chip_num;
	int                         gpio_num;
	int                         active;
	struct gpiod_line_request  *request;
}led_t;

static led_t leds_info[LED_MAX] = 
{
	{"red", 0, 11, ACTIVE_LOW, NULL},
	{"blue", 0, 10,  ACTIVE_LOW, NULL},
	{"green", 4, 9,  ACTIVE_LOW, NULL},
};

typedef struct leds_s
{
	led_t          *leds;
	int             count;
}leds_t;


int init_led(void);
int term_led(void);
int turn_led(int which, int cmd);
static inline void msleep(unsigned long ms);
void sig_handler(int signum);




#endif
