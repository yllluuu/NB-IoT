/********************************************************************************
 *      Copyright:  (C) 2024 yanglu 
 *                  All rights reserved.
 *
 *       Filename:  at_cmd.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(15/07/24)
 *         Author:  yanglu <yanglu@2012174595@qq.com>
 *      ChangeLog:  1, Release initial version on "15/07/24 14:14:27"
 *                 
 ********************************************************************************/

#ifndef  _AT_CMD_H_
#define  _AT_CMD_H_

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "comport.h"
#include "logger.h"

#define ATCMD_REPLY_LEN         1024
#define ATCMD_LEN               256

#define AT_OKSTR          "\r\nOK\r\n"
#define AT_ERRSTR         "\r\nERROR\r\n"

#define AT_SUFFIX         "\r\n"

extern int       g_send_event;
extern int       g_leds_event;

enum
{
	ATRES_ERROR,
	ATRES_EXPECT,
	ATRES_TIMEOUT,
};


int atcmd_send(comport_t *comport, char *at, unsigned long timeout, char *expect, char *error, char *reply, int size);


int atcmd_check_ok(comport_t *comport, char *at, unsigned long timeout);

int atcmd_check_value(comport_t *comport, char *at, unsigned long timeout, char *reply, int size);

int parser_request_value(char *buf, char *key, char *value, int size);

int atcmd_send_request(comport_t *comport, char *at, unsigned long timeout, char *reply, int size);

#endif
