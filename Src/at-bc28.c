/*
 * NB-IoT_test.c
 *
 *  Created on: Jul 2, 2024
 *      Author: 杨璐
 */
#include "at-bc28.h"

nbiot_conf_t		g_nbconf;

int bc28_check_at(comport_t *comport)
{
	if(atcmd_check_OK(comport, "AT", 500)<0)
	{
		printf("AT command test failed,try again...\r\n");
		return -1;
	}
	else
		printf("AT command is normal.\r\n");
	return 0;
}

int bc28_reset(comport_t *comport)
{
	if(atcmd_send(comport, "AT+NRB", 1000, "OK", AT_ERRSTR, NULL, 0)<0)
	{
		printf("send AT command to reset NB-IoT failed\r\n");
		return -1;
	}
	else
		printf("bc28 module Reset OK\r\n");
	return 0;
}

int bc28_get_manuf(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CGMI", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		printf("View module manufacturers failed,try again...\r\n");
		return -1;
	}
	else
	{
		printf("View module manufacturers OK.\r\n");
	}
	return 0;
}

int bc28_get_module(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CGMM", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		printf("View module module failed,try again...\r\n");
		return -1;
	}
	else
	{
		printf("View module model OK.\r\n");
	}
	return 0;
}

int bc28_check_imei(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CGSN=1", 500, reply_buf, size)<0)
	{
		printf("Check module IMEI number is not normal\r\n");
		return -1;
	}
	printf("Check module IMEI number is normal\r\n");

	return 0;
}

int bc28_check_simcd(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CIMI", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		printf("SIM card does not exist\r\n");
		return -1;
	}
	printf("SIM card exists\r\n");

	return 0;
}

int bc28_set_autocnt(comport_t *comport)
{
	if(atcmd_check_OK(comport, "AT+NCONFIG=AUTOCONNECT,TRUE",500)<0)
	{
		printf("Auto connect failed\r\n");
		return -1;
	}
	printf("Auto connect OK\r\n");

	return 0;
}

int bc28_check_cfun(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CFUN?", 500, reply_buf, size)<0)
	{
		printf("The radio is not turn on\r\n");
		return -1;
	}
	printf("The radio is turned on\r\n");

	return 0;
}

int bc28_check_csq(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CSQ", 500, reply_buf, size)<0)
	{
		printf("The module signal test failed,try again...\r\n");
		return -1;
	}
	printf("The module signal test is normal,reply_buf:%s\r\n",reply_buf);

	return 0;
}

int bc28_set_attach_net(comport_t *comport)
{
	if(atcmd_check_OK(comport, "AT+CGATT=1", 500)<0)
	{
		printf("The module attachment network test failed,try again...\r\n");
		return -1;
	}
	printf("The module attachment network test is normal\r\n");

	return 0;
}

int bc28_check_attach_net(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CGATT?", 500, reply_buf, size)<0)
	{
		printf("The module attachment network test failed,try again...\r\n");
		return -1;
	}
	printf("The module attachment network test is normal\r\n");

	return 0;
}

int bc28_check_reg_status(comport_t *comport, char *reply_buf,size_t size)
{
	if(atcmd_check_value(comport, "AT+CEREG?", 500,reply_buf,size)<0)
	{
		printf("The network registration status is abnormal,try again...\r\n");
		return -1;
	}
	printf("The network registration status is normal\r\n");

	return 0;
}

int bc28_check_ip(comport_t *comport, char *reply_buf,size_t size)
{
	if(atcmd_check_value(comport, "AT+CGPADDR", 500,reply_buf,size)<0)
	{
		printf("Obtaining an IP address is abnormal,try again...\r\n");
		return -1;
	}
	printf("Obtaining an IP address is normal\r\n");

	return 0;
}

int bc28_set_ip_port(comport_t *comport, char *reply_buf, size_t size)
{
	char		buf[256]={0};
	snprintf(buf,sizeof(buf),"AT+NCDP=%s",NCDP);
	if(atcmd_send(comport, buf, 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		printf("The module fails to connect to the cloud platform failed, try again...\r\n");
		return -1;
	}
	printf("The module connect to the cloud platform successfully.\r\n");

	return 0;
}

int bc28_check_ip_port(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+NCDP?", 500, reply_buf, size)<0)
	{
		printf("The cloud platform is incorrectly configured, try again...\r\n");
		return -1;
	}
	printf("The cloud platform is correctly configured.\r\n");

	return 0;
}

int bc28_check_iot(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+NMSTATUS?", 500, reply_buf, size)<0)
	{
		printf("Failed to register the telecom cloud platform,try again...\r\n");
		return -1;
	}
	printf("Successfully registered on the telecom cloud platform\r\n");

	return 0;
}

int nb_reset_ok(comport_t *comport)
{
	if(bc28_check_at(comport)<0)
		return -1;
	if(bc28_reset(comport)<0)
		return -1;

	return 0;
}

int nb_hdw_ok(comport_t *comport)
{
	if(bc28_get_manuf(comport, g_nbconf.manufacturers, ATBUF_SIZE)<0)
		return -1;

	if(bc28_get_module(comport, g_nbconf.model, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_imei(comport, g_nbconf.imei, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_simcd(comport, g_nbconf.sim, ATBUF_SIZE)<0)
		return -1;

	return 0;
}

int nb_conf_ok(comport_t *comport)
{
	char 		reply_buf[ATBUF_SIZE];

	if(bc28_set_autocnt(comport)<0)
		return -1;

	if(bc28_check_cfun(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_csq(comport, g_nbconf.csq, ATBUF_SIZE)<0)
		return -1;

	if(bc28_set_attach_net(comport)<0)
		return -1;

	if(bc28_check_attach_net(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_reg_status(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_ip(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_set_ip_port(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_ip_port(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	if(bc28_check_iot(comport, reply_buf, ATBUF_SIZE)<0)
		return -1;

	return 0;
}

