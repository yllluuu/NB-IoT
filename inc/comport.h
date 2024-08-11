/********************************************************************************
 *      Copyright:  (C) 2024 yanglu 
 *                  All rights reserved.
 *
 *       Filename:  comport.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(14/07/24)
 *         Author:  yanglu <yanglu@2012174595@qq.com>
 *      ChangeLog:  1, Release initial version on "14/07/24 14:26:10"
 *                 
 ********************************************************************************/

#ifndef _COMPORT_H_
#define _COMPORT_H_  


#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <getopt.h>
#include  <fcntl.h>
#include  <errno.h>
#include  <termios.h>
#include  <sys/stat.h>
#include  <sys/wait.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/select.h>
#include  <pthread.h>

#define   CONFIG_DEF_FRAGSIZE    128
#define   TIMEOUT       5
#define   RECEIVE_SIZE    512

extern    pthread_mutex_t      comport_mutex;

typedef  struct  comport_s{
	int               fd;
    char              devname[32];
	long              baudrate;
	unsigned char     databit,parity,stopbit,flowctrl;
	int               fragsize;
}comport_t;

typedef struct rece_flags_s
{
	char       leds_event_buf[RECEIVE_SIZE];
	char       send_event_buf[RECEIVE_SIZE];
}rece_flags_t;

extern rece_flags_t    g_rece_flags;

extern int comport_open(comport_t *comport, const char *devname, long baudrate, const char *settings);

extern void comport_close(comport_t *comport);

extern int comport_send(comport_t *comport, char *data, int data_bytes);

extern int comport_recv(comport_t *comport, char *buf, int buf_size, unsigned long timeout);


#endif
