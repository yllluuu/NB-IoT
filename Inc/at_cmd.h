/********************************************************************************
 *      Copyright:  (C) 2024 Niuchunmin 
 *                  All rights reserved.
 *
 *       Filename:  at_cmd.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(15/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
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

extern int       SEND_EVENT_G;
extern int       LEDS_EVENT_G;

enum
{
	ATRES_ERROR,
	ATRES_EXPECT,
	ATRES_TIMEOUT,
};


int send_atcmd(comport_t *comport, char *at, unsigned long timeout, char *expect, char *error, char *reply, int size);


int send_atcmd_check_ok(comport_t *comport, char *at, unsigned long timeout);

int send_atcmd_check_value(comport_t *comport, char *at, unsigned long timeout, char *reply, int size);

int parser_request_value(char *buf, char *key, char *value, int size);

int send_atcmd_request(comport_t *comport, char *at, unsigned long timeout, char *reply, int size);

#endif
