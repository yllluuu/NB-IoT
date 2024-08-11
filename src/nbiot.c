/*********************************************************************************
 *      Copyright:  (C) 2024 yanglu 
 *                  All rights reserved.
 *
 *       Filename:  nbiot.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/07/24)
 *         Author:  yanglu <yanglu@2012174595@qq.com>
 *      ChangeLog:  1, Release initial version on "17/07/24 10:46:46"
 *                 
 ********************************************************************************/

#include "nbiot.h"

int g_temp_interval = 10;
void print_usage(char *progname);

static inline void msleep(unsigned long ms)
{
	struct timespec cSleep;
	unsigned long   ulTmp;

	cSleep.tv_sec = ms / 1000;
	if( cSleep.tv_sec == 0)
	{
		ulTmp = ms * 10000;
		cSleep.tv_nsec = ulTmp * 100;
	}
	else
	{
		cSleep.tv_nsec = 0;
	}

	nanosleep(&cSleep, 0);
}

void init_mutex() {
	if (pthread_mutex_init(&comport_mutex, NULL) != 0) {
		log_error ("Mutex init failed\n");
		exit(EXIT_FAILURE);
	}
}
void float_to_hex_string(float f, char hexStr[9]) {
	unsigned char *bytePtr = (unsigned char*)&f;
	sprintf(hexStr, "%02X%02X%02X%02X", bytePtr[3], bytePtr[2],               bytePtr[1],     bytePtr[0]);
}

void *nbiot_mgr_thread(void *arg)
{
	nb_config_t*     nbiot_data = (nb_config_t*)arg;
	int              rv;
	nbiot_data->current_state = STATUS_INIT;


	while(1)
	{
		switch(nbiot_data->current_state)
		{
			case STATUS_INIT:
				rv = nb_reset_ok(&nbiot_data->comport);
				if( rv<0 )
				{
					log_error ("debug:STATUS_INIT failed.\n");
					break;
				}
				else
				{
					nbiot_data->current_state++;
					break;
				}
			case STATUS_PRESEND:
				rv = nb_hdw_ok(&nbiot_data->comport);
				if( rv<0 )
				{
					log_error ("debug:STATUS_PRESEND failed.\n");
					break;
				}
				else
				{
					nbiot_data->current_state++;
					break;
				}
			case STATUS_CONFIG:
				rv = nb_conf_ok(&nbiot_data->comport);
				if( rv<0 )
				{
					log_error ("debug : STATUS_CONFIG failed.\n");
					break;
				}
				else
				{
					nbiot_data->current_state++;
					break;
				}
			case STATUS_READY:

				log_info ("READY!!!!!!\n");
				sleep(1);
				break;
			default:
				break;
		}
	}

	return NULL;
}


void *report_data(void *arg)
{
	nb_config_t*      nbiot_data = (nb_config_t*)arg;
	int               rv;
	char              tx_buffer[DATA_SIZE];
	float             temp;
	float             humi;
	char              hex_str_temp[9];
	char              hex_str_humi[9];


	log_debug ("test state:%d\n", nbiot_data->current_state);

	while(1)
	{
		if(nbiot_data->current_state != STATUS_READY)
		{
			msleep(10);
			continue;
		}

		memset(hex_str_temp, 0, sizeof(hex_str_temp));
		memset(hex_str_humi, 0, sizeof(hex_str_humi));

		rv = sht2x_get_temp_humi(SHT2X_DEVNAME, &temp, &humi);
		if( rv<0 )
		{   
			log_error("SHT2X get temperature and humidity failure\n");
		}   

		float_to_hex_string(temp, hex_str_temp);
		float_to_hex_string(humi, hex_str_humi);

		snprintf(tx_buffer, sizeof(tx_buffer), "AT+QLWULDATAEX=13,0200250008%s%s,0x0100\r\n", hex_str_temp, hex_str_humi);
		rv = atcmd_check_ok(&nbiot_data->comport, tx_buffer, 5000);
		if( rv<0 )
		{
			log_error ("Send AT data failure.\n");
		}
		else
			log_info ("Send successful:%s\n",tx_buffer);
		sleep(g_temp_interval);
	}
}


void *receive_data(void *arg)
{
	nb_config_t*      nbiot_data = (nb_config_t*)arg;
	int               rv;
	int               bytes = 0;
	char              rx_buffer[1024];
	comport_t         comport;

	comport = nbiot_data->comport;


	while(1)
	{

		memset(rx_buffer, 0, sizeof(rx_buffer));

		bytes = comport_recv(&comport, rx_buffer, sizeof(rx_buffer), 10);
WAIT_NEWDATA:
		rv = comport_recv(&comport, rx_buffer+bytes, sizeof(rx_buffer)-bytes, 10);
		if( rv>0 )
		{
			bytes += rv;
			goto WAIT_NEWDATA;
		}

		rx_buffer[bytes] = '\0';

		if( strstr(rx_buffer, "+NNMI:") )
		{
			memset(g_rece_flags.leds_event_buf, 0, sizeof(g_rece_flags.leds_event_buf));
			//copy receive data，excute asyn_led_process
			strcpy(g_rece_flags.leds_event_buf, rx_buffer);
			g_leds_event = 1;
			log_debug ("debug:led parser:%d\n",g_leds_event);
		}
		else
		{
			memset(g_rece_flags.send_event_buf, 0, sizeof(g_rece_flags.send_event_buf));
			//copy receive data, AT reply
			strcpy(g_rece_flags.send_event_buf, rx_buffer);
			if(nbiot_data->current_state == STATUS_READY)
			{
				g_send_event = 1;
			}
		}

	}
}

void *asyn_process_leds(void *arg)
{

	while(1)
	{
		if(g_leds_event ==1)
		{
			if (strstr(g_rece_flags.leds_event_buf, "0101"))
			{
				turn_led(LED_R, ON);
			}
			else if(strstr(g_rece_flags.leds_event_buf, "0100"))
			{
				turn_led(LED_R, OFF);
			}
		}
		else
		{
			continue;
		}
		g_leds_event = 0;
	}
}




void print_usage(char *progname)
{
    printf("%s:usage:\n",progname);
    printf ("%s is nbiot program running on IMX6ULL\n", progname);
    printf("-n(--devname) :specify serial port devname.\n");
    printf ("-t(--time)  :temp upload time.Default 10s\n");
    printf ("-b(--background) :running in background mode\n");
    printf("-h(--help)   :print this help information.\n");
    printf ("test:make successfully?\n");

    return ;
}
