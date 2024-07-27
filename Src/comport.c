/*********************************************************************************
 *      Copyright:  (C) 2024 niuchunmin
 *                  All rights reserved.
 *
 *       Filename:  comport.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(14/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "14/07/24 16:10:46"
 *                 
 ********************************************************************************/

#include "comport.h"

//#define CONFIG_PRINT_LOGGER
#define CONFIG_PRINT_STDOUT

#if ( defined CONFIG_PRINT_LOGGER )
#include "logger.h"
#define dbg_print(format,args...) log_error(format, ##args)

#elif ( defined CONFIG_PRINT_STDOUT )
#define dbg_print(format,args...) printf(format, ##args)

#else
#define dbg_print(format,args...) do{} while(0);
#endif

static inline void set_settings(comport_t *comport, const char *settings);

rece_flags_t       g_rece_flags = {{0},{0}};
pthread_mutex_t    comport_mutex = PTHREAD_MUTEX_INITIALIZER;


int comport_open(comport_t *comport, const char *devname, long baudrate, const char *settings)
{
	int                         rv = -1;
	struct termios              old_cfg, new_cfg;
	int                         old_flags;
	long                        tmp;

	if( !comport || !devname )
	{
		dbg_print("invalid input arugments\n");
		return -1;
	}

	/* --------------Init comport------------- */
	memset(comport, 0, sizeof(*comport));
	strncpy(comport->devname, devname, sizeof(comport->devname));
	comport->baudrate = baudrate;
	comport->fd = -1;
	comport->fragsize = CONFIG_DEF_FRAGSIZE;
	set_settings(comport, settings);

	/* -------------Open comport-------------- */
	if( !strstr(comport->devname, "tty") )
	{
		dbg_print("comport device \"%s\" is not tty device\n", comport->devname);
		return -2;
	}

	comport->fd = open(comport->devname, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( comport->fd<0 )
	{
		dbg_print("comport open \"%s\" failed:%s\n", comport->devname, strerror(errno));
		return -3;
	}

	if(   (-1 != (old_flags = fcntl(comport->fd, F_GETFL, 0)))
			&& (-1 != fcntl(comport->fd, F_SETFL, old_flags & ~O_NONBLOCK)) )
	{
		/*  Flush input and output */
		tcflush(comport->fd, TCIOFLUSH);
	}
	else
	{
		rv = -4;
		goto cleanup;
	}

	if (0 != tcgetattr(comport->fd, &old_cfg))
	{
		rv = -5;
		goto cleanup;
	}
	/* -------------configure comport---------------- */
	memset(&new_cfg, 0, sizeof(new_cfg));
	new_cfg.c_cflag &= ~CSIZE;
	new_cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	new_cfg.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	new_cfg.c_oflag &= ~(OPOST);

	/* --------------set databit------------- */
	switch (comport->databit)
	{
		case 0x07:
			new_cfg.c_cflag |= CS7;
			break;
		case 0x06:
			new_cfg.c_cflag |= CS6;
			break;
		case 0x05:
			new_cfg.c_cflag |= CS5;
			break;
		default:
			new_cfg.c_cflag |= CS8;
			break;
	}

	/* --------------set parity------------- */
	switch (comport->parity)
	{
		case 0x01:   
			new_cfg.c_cflag |= (PARENB | PARODD);
			new_cfg.c_cflag |= (INPCK | ISTRIP);
			break;
		case 0x02:    
			new_cfg.c_cflag |= PARENB;
			new_cfg.c_cflag &= ~PARODD;;
			new_cfg.c_cflag |= (INPCK | ISTRIP);
			break;
		case 0x03:
			new_cfg.c_cflag &= ~PARENB;
			new_cfg.c_cflag &= ~CSTOPB;
			break;
		default:
			new_cfg.c_cflag &= ~PARENB;
	}

	/* --------------set stopbit------------- */
	if (0x01 != comport->stopbit)
	{
		new_cfg.c_cflag |= CSTOPB;
	}
	else
	{
		new_cfg.c_cflag &= ~CSTOPB;
	}

	/* ------------set conctrl--------------- */
	switch (comport->flowctrl)
	{
		case 1:       
		case 3:
			new_cfg.c_cflag &= ~(CRTSCTS);
			new_cfg.c_iflag |= (IXON | IXOFF);
			break;
		case 2:       
			new_cfg.c_cflag |= CRTSCTS;
			new_cfg.c_iflag &= ~(IXON | IXOFF);
			break;
		default:      
			new_cfg.c_cflag &= ~(CRTSCTS);
			new_cfg.c_iflag &= ~(IXON | IXOFF);
			break;
	}

	/*--------------set baudrate-------------  */
	switch (comport->baudrate)
	{
		case 4000000:
			tmp = B4000000;
			break;
		case 3500000:
			tmp = B3500000;
			break;
		case 3000000:
			tmp = B3000000;
			break;
		case 2500000:
			tmp = B2500000;
			break;
		case 2000000:
			tmp = B2000000;
			break;
		case 1500000:
			tmp = B1500000;
			break;
		case 115200:
			tmp = B115200;
			break;
		case 9600:
			tmp = B9600;
			break;
		default:
			tmp = B115200;

	}
	cfsetispeed(&new_cfg, tmp);
	cfsetospeed(&new_cfg, tmp);

	/* ------------set comport timeout settings----------- */
	new_cfg.c_cc[VMIN] = 0;
	new_cfg.c_cc[VTIME] = 0;

	tcflush(comport->fd, TCIFLUSH);
	if (0 != tcsetattr(comport->fd, TCSANOW, &new_cfg))
	{
		rv = -6;          
		goto cleanup;
	}

	rv = comport->fd;
cleanup:
	return rv;

}


void comport_close(comport_t *comport)
{
	if( !comport )
	{
		dbg_print("invalid input arugments\n");
		return ;
	}

	if ( comport->fd >= 0 )
	{
		close(comport->fd);
	}

	comport->fd = -1;
	return ;
}


int comport_send(comport_t *comport, char *data, int data_bytes)
{
	char                       *ptr;
	int                         left, bytes = 0;
	int                         rv = 0;

	if( !comport || !data || data_bytes<=0 )
	{
		dbg_print("invalid input arugments\n");
		return -1;
	}

	if( comport->fd < 0 )
	{
		dbg_print("Serail port not opened\n");
		return -2;
	}

	tcflush( comport->fd, TCIFLUSH);

	ptr = data;
	left = data_bytes;

	while( left > 0 )
	{

		bytes = left>comport->fragsize ? comport->fragsize : left;

		rv = write(comport->fd, ptr, bytes);
		if( rv<0 )
		{
			rv = -3;
			break;
		}

		left -= rv;
		ptr += rv;
	}

	return rv;
}

int comport_recv(comport_t *comport, char *buf, int buf_size, unsigned long timeout)
{
	fd_set                      rdfds, exfds;
	struct timeval              to, *to_ptr = NULL;
	int                         ret, rv = 0;
	int                         bytes = 0;

	if ( !comport || !buf || buf_size<=0 )
	{
		dbg_print("invalid input arugments\n");
		return -1;
	}

	if ( comport->fd < 0 )
	{
		dbg_print("Serail port not opened\n");
		return -2;
	}

	memset(buf, 0, buf_size);

	FD_ZERO(&rdfds);
	FD_ZERO(&exfds);
	FD_SET(comport->fd, &rdfds);
	FD_SET(comport->fd, &exfds);

	if( TIMEOUT != timeout )
	{
		to.tv_sec = (time_t) (timeout / 1000);
		to.tv_usec = (long)(1000 * (timeout % 1000));
		to_ptr = &to;
	}

	while( 1 )
	{

		ret = select(comport->fd+1, &rdfds, 0, &exfds, to_ptr);
		if( ret<0 )
		{
			dbg_print("comport select() failed: %s\n", strerror(errno));
			rv = EINTR==errno ? 0 : -3;
			break;
		}
		else if( 0 == ret ) 
		{
			break;
		}

		ret = read(comport->fd, buf+bytes, buf_size-bytes);
		if(ret <= 0)
		{
			dbg_print("comport read() failed: %s\n", strerror(errno));
			break;
		}

		bytes += ret;
		if( bytes >= buf_size )
			break;

		to.tv_sec = 0;
		to.tv_usec = 10000;
		to_ptr = &to;
	}

	if( !rv )
		rv = bytes;

	return rv;
}

static inline void set_settings(comport_t * comport, const char *settings)
{
	if( !settings || !comport )
	{
		dbg_print("invalid input arugments\n");
		return ;
	}

	switch (settings[0])      
	{
		case '7':
			comport->databit = 7;
			break;
		case '8':
		default:
			comport->databit = 8;
			break;
	}

	switch (settings[1])     
	{
		case 'O':
		case 'o':
			comport->parity = 1;
			break;
		case 'E':
		case 'e':
			comport->parity = 2;
			break;
		case 'S':
		case 's':
			comport->parity = 3;
			break;
		case 'N':
		case 'n':
		default:
			comport->parity = 0;
			break;
	}

	switch (settings[2])     
	{
		case '0':
			comport->stopbit = 0;
			break;
		case '1':
		default:
			comport->stopbit = 1;
			break;
	}

	switch (settings[3])       
	{
		case 'S':
		case 's':
			comport->flowctrl = 1;
			break;
		case 'H':
		case 'h':
			comport->flowctrl = 2;
			break;
		case 'B':
		case 'b':
			comport->flowctrl = 3;
			break;
		case 'N':
		case 'n':
		default:
			comport->flowctrl = 0;
			break;
	}
}
