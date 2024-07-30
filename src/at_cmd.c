/*********************************************************************************
 *      Copyright:  (C) 2024 niuchunmin
 *                  All rights reserved.
 *
 *       Filename:  at_cmd.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(16/07/24)
 *         Author:  Niu Chunmin <2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "16/07/24 20:21:26"
 *                 
 ********************************************************************************/

#include "at_cmd.h"

int    g_send_event = 0;
int    g_leds_event = 0;

int atcmd_send(comport_t *comport, char *at, unsigned long timeout,  char *expect, char *error, char *reply, int size)
{
	int                    i, rv = 0;
	int                    res = ATRES_TIMEOUT;
	int                    bytes = 0;
	size_t                 length;
	char                   buf[ATCMD_REPLY_LEN] = {'\0'};
	char                   atcmd[ATCMD_LEN] = {'\0'};

	if( !comport || !at )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	if( comport->fd <= 0 )
	{
		log_error("comport[%s] not opened\n");
		return -2;
	}

	/*  flushes both data received but not read, and data written but not transmitted in serial port */
	tcflush(comport->fd, TCIOFLUSH);

	snprintf(atcmd, sizeof(atcmd), "%s%s", at, AT_SUFFIX);
	rv=comport_send( comport, atcmd, strlen(atcmd) );
	if(rv < 0)
	{
		log_error("send AT command \"%s\" to \"%s\" failed, rv=%d\n", at, comport->devname, rv);
		return -3;
	}

	res = ATRES_TIMEOUT;
	memset( buf, 0, sizeof(buf) );


	for(i=0; i<timeout/10; i++)
	{
		if( g_send_event != 1 )
		{
			usleep(1000);
			break;
		}

		//判断是否是你发送的命令

		if( !(strstr(g_rece_flags.send_event_buf, at)) )
		{
			log_error("Received is not send AT command.\n");
			return -8;
		}
		length = strlen(g_rece_flags.send_event_buf);
		if(length < 0)
		{
			log_error("send AT command \'%s\' to \'%s\' failed, rv=%d\n", at, comport->devname, rv);
			return -3;
		}


		if( expect && strstr(g_rece_flags.send_event_buf, expect) )
		{
			log_debug("send AT command \"%s\" and got reply \"OK\"\n", at);
			res = ATRES_EXPECT;
			break;
		}

		if( error && strstr(g_rece_flags.send_event_buf, error) )
		{
			log_debug("send AT command \"%s\" and got reply \"ERROR\"\n", at);
			res = ATRES_ERROR;
			break;
		}
	}

	if( length > 0 )
		log_trace("AT command reply:%s", g_rece_flags.send_event_buf);

	if( reply && size>0 )
	{
		bytes = strlen(g_rece_flags.send_event_buf)>size ? size : strlen(g_rece_flags.send_event_buf);
		memset(reply, 0, size);
		strncpy(reply, g_rece_flags.send_event_buf, length);

		log_debug("copy out AT command \"%s\" reply message: \n%s", at, reply);
		g_send_event = 0;
	}

	return res;
}



int atcmd_check_ok(comport_t *comport, char *at, unsigned long timeout)
{
	int                     rv;

	if( !comport || !at )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	rv=atcmd_send(comport, at, timeout, AT_OKSTR, AT_ERRSTR, NULL, 0);
	if( ATRES_EXPECT == rv )
	{
		return 0;
	}
	else
	{
		return -2;
	}
}


int atcmd_check_value(comport_t *comport, char *at, unsigned long timeout, char *reply, int size)
{
	int                     rv, len;
	char                    buf[ATCMD_REPLY_LEN];
	char                   *ptr_start = buf;
	char                   *ptr_end;

	if( !comport || !at || !reply || size<=0 )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	rv = atcmd_send(comport, at, timeout, AT_OKSTR, AT_ERRSTR, buf, ATCMD_REPLY_LEN);
	if( rv <= 0 )
	{
		return -2;
	}

	/*  Skip the echo back command line */
	if( !strncmp(buf, at, strlen(at)) )
	{
		ptr_start=strchr(buf, '\n');
		if( !ptr_start )
		{
			log_error("reply message got wrong\n");
			return -3;
		}

		ptr_start++;   /*  skip '\n'  */
	}

	/*  find end reply string "\r\nOK\r\n"  */
	ptr_end = strstr(ptr_start, AT_OKSTR);
	if( ptr_end )
	{
		len = ptr_end - ptr_start;
	}
	else
	{
		len = strlen(buf) - (ptr_start-buf);
	}

	memset(reply, 0, size);

	len = len>size ? size : len;
	memcpy(reply, ptr_start, len);

	return 0;
}


int parser_request_value(char *buf, char *key, char *value, int size)
{
	char                   *ptr;
	int                    i = 0;

	if( !buf || !key || size<=0 )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	ptr = strstr(buf, key);
	if( !ptr )
	{
		log_debug("Not found key \"%s\" in %s\n", key, buf);
		return -2;
	}

	ptr=strchr(ptr, ':');  /*  found ':' before the value */
	if( !ptr )
	{
		log_debug("Not found ':' before value\n");
		return -3;
	}
	ptr++;   

	if( *ptr == '\"' ) 
		ptr++;

	memset(value, 0, size);
	while(*ptr!='\r' && i<size-1)
	{
		if( !isspace(*ptr) && *ptr!='\"') /*  skip space,\r,\n ...  */
			value[i++] = *ptr;
		ptr ++;
	}

	ptr++; /*  skip  */

	return 0;
}

int atcmd_send_request(comport_t *comport, char *at, unsigned long timeout, char *reply, int size)
{
	int                     i = 0;
	int                     rv;
	char                    buf[ATCMD_REPLY_LEN];
	char                   *ptr;

	if( !comport || !at || !reply || size<=0 )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	rv = atcmd_send(comport, at, timeout, AT_OKSTR, AT_ERRSTR, buf, ATCMD_REPLY_LEN);
	if( rv <= 0 )
	{
		return -2;
	}

	ptr=strchr(buf, '+');  
	if( !ptr )
	{
		log_error("reply message got wrong\n");
		return -3;
	}
	ptr++;   


	ptr=strchr(buf, ':');  
	if( !ptr )
	{
		log_error("reply message got wrong\n");
		return -3;
	}
	ptr++;  

	if( *ptr == '\"' ) 
		ptr++;

	memset(reply, 0, size);
	while(*ptr!='\r' && i<size-1)
	{
		if( !isspace(*ptr) && *ptr!='\"') 
			reply[i++] = *ptr;
		ptr ++;
	}

	return 0;
}
