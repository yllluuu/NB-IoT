/*
 * NB-IoT_test.c
 *
 *  Created on: Jul 2, 2024
 *      Author: yanglu
 */
#include "at-bc28.h"

nbiot_conf_t		nbconf;

int bc28_check_at(comport_t *comport)
{
	if(atcmd_check_ok(comport, "AT", 500)<0)
	{
		log_error("AT command test failed,try again...\r\n");
		return -1;
	}
	else
		log_info("AT command is normal.\r\n");
	return 0;
}

int bc28_reset(comport_t *comport)
{
	if(atcmd_send(comport, "AT+NRB", 5000, "OK", AT_ERRSTR, NULL, 0)<0)
	{
		log_error("send AT command to reset NB-IoT failed\r\n");
		return -1;
	}
	else
		log_info("BC28 module Reset OK\r\n");
	return 0;
}

int bc28_get_manuf(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CGMI", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		log_error("View module manufacturers failed,try again...\r\n");
		return -1;
	}
	else
	{
		strncpy(nbconf.manufacturers,reply_buf,strlen(reply_buf));
		log_info("View module manufacturers OK.\r\n");
	}
	return 0;
}

int bc28_get_module(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CGMM", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		log_error("View module module failed,try again...\r\n");
		return -1;
	}
	else
	{
		strncpy(nbconf.model,reply_buf,size);
		log_info("View module model OK.\r\n");
	}
	return 0;
}

int bc28_check_imei(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CGSN=1", 500, reply_buf, size)<0)
	{
		log_error("Check module IMEI number is not normal\r\n");
		return -1;
	}
	strncpy(nbconf.imei,reply_buf,size);
	log_info("Check module IMEI number is normal\r\n");

	return 0;
}

int bc28_check_simcd(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_send(comport, "AT+CIMI", 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		log_error("SIM card does not exist\r\n");
		return -1;
	}
	strncpy(nbconf.sim,reply_buf,size);
	log_info("SIM card exists\r\n");

	return 0;
}

int bc28_set_autocnt(comport_t *comport)
{
	if(atcmd_check_ok(comport, "AT+NCONFIG=AUTOCONNECT,TRUE",500)<0)
	{
		log_error("Auto connect failed\r\n");
		return -1;
	}
	log_info("Auto connect OK\r\n");

	return 0;
}

int bc28_check_cfun(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CFUN?", 500,reply_buf,size)<0)
	{
		log_error("The radio is not turn on\r\n");
		return -1;
	}
	log_info("The radio is turned on\r\n");

	return 0;
}

int bc28_check_csq(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CSQ", 500,reply_buf,size)<0)
	{
		log_error("The module signal test failed,try again...\r\n");
		return -1;
	}
	strncpy(nbconf.csq,reply_buf,size);
	log_info("The module signal test is normal\r\n");

	return 0;
}

int bc28_set_attach_net(comport_t *comport)
{
	if(atcmd_check_ok(comport, "AT+CGATT=1", 500)<0)
	{
		log_error("The module attachment network test failed,try again...\r\n");
		return -1;
	}
	log_info("The module attachment network test is normal\r\n");

	return 0;
}

int bc28_check_attach_net(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CGATT?", 500,reply_buf,size)<0)
	{
		log_error("The module attachment network test failed,try again...\r\n");
		return -1;
	}
	log_info("The module attachment network test is normal\r\n");

	return 0;
}

int bc28_check_reg_status(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CEREG?", 500,reply_buf,size)<0)
	{
		log_error("The network registration status is abnormal,try again...\r\n");
		return -1;
	}
	log_info("The network registration status is normal\r\n");

	return 0;
}

int bc28_check_ip(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+CGPADDR", 500,reply_buf,size)<0)
	{
		log_error("Obtaining an IP address is abnormal,try again...\r\n");
		return -1;
	}
	log_info("Obtaining an IP address is normal\r\n");

	return 0;
}

int bc28_set_ip_port(comport_t *comport, char *reply_buf, size_t size)
{
	char        buf[256] = {0};

	snprintf(buf, 256, "AT+NCDP=%s",IP_PORT);
	if(atcmd_send(comport, buf, 500, AT_OKSTR, AT_ERRSTR, reply_buf, size)<0)
	{
		log_error("The module fails to connect to the cloud platform failed, try again...\r\n");
		return -1;
	}
	log_info("The module connect to the cloud platform successfully.\r\n");

	return 0;
}

int bc28_check_ip_port(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+NCDP?", 500,reply_buf,size)<0)
	{
		log_error("The cloud platform is incorrectly configured, try again...\r\n");
		return -1;
	}
	log_info("The cloud platform is correctly configured.\r\n");

	return 0;
}

int bc28_check_iot(comport_t *comport, char *reply_buf, size_t size)
{
	if(atcmd_check_value(comport, "AT+NMSTATUS?", 500,reply_buf,size)<0)
	{
		log_error("Failed to register the telecom cloud platform,try again...\r\n");
		return -1;
	}
	log_info("Successfully registered on the telecom cloud platform\r\n");

	return 0;
}

int nb_reset_ok(comport_t *comport)
{
	if(bc28_check_at(comport)<0)
		return -1;

	return 0;
}

int nb_hdw_ok(comport_t *comport)
{
	char 		reply_buf[256];

	if(bc28_get_manuf(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_get_module(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_imei(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_simcd(comport, reply_buf, ATCMD_REPLY_LEN))
		return -1;

	return 0;
}

int nb_conf_ok(comport_t *comport)
{
	char 		reply_buf[256];

	if(bc28_set_autocnt(comport)<0)
		return -1;

	if(bc28_check_cfun(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_csq(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_set_attach_net(comport)<0)
		return -1;

	if(bc28_check_attach_net(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_reg_status(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_ip(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_set_ip_port(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_ip_port(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	if(bc28_check_iot(comport, reply_buf, ATCMD_REPLY_LEN)<0)
		return -1;

	return 0;
}

