/*
 * NB-IoT_test.c
 *
 *  Created on: Jul 2, 2024
 *      Author: 杨璐
 */
#include "NB-IoT_test.h"
#include "usart.h"
#include "string.h"
#include "at_cmd.h"

NBiot_conf_t		NBconf;

int NB_RSET_OK()
{
	int		timeout = 200;
	char	reply_buf[256];
/*
	if(atcmd_send("AT+NRB", 1000,reply_buf,sizeof(reply_buf)))
	{
		printf("Reset NB-IoT failed\r\n");
		return -1;
	}
	else
		printf("Reset NB-IoT OK\r\n");
	*/

	if(atcmd_send("AT", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("AT command test failed,try again...\r\n");
		return -1;
	}
	else
		printf("AT command is normal.\r\n");

	if(atcmd_send("AT+CGMI", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("View module manufacturers failed,try again...\r\n");
		return -1;
	}
	else
	{
		strcpy(NBconf.manufacturers,reply_buf);
		printf("View module manufacturers OK.NBconf.manufacturers:%s\r\n",NBconf.manufacturers);
	}

	if(atcmd_send("AT+CGMM", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("View module model failed,try again...\r\n");
		return -1;
	}
	else
	{
		strcpy(NBconf.model,reply_buf);
		printf("View module model OK.NBconf.model:%s,reply_buf:%s\r\n",NBconf.model,reply_buf);
	}

	return 0;
}

int NB_HDW_OK()
{
	int 	timeout = 200;
	char	reply_buf[256];

	if(atcmd_check_value("AT+CGSN=1",timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("Check module IMEI number is not normal\r\n");
		return -1;
	}
	strcpy(NBconf.IMEI,reply_buf);
	printf("Check module IMEI number is normal,IMEI:%s,%s\r\n",NBconf.IMEI,reply_buf);

	if(atcmd_send("AT+CIMI",timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("SIM card does not exist\r\n");
		return -1;
	}
	strcpy(NBconf.SIM,reply_buf);
	printf("SIM card exists\r\n");

	return 0;
}

int NB_CONF_OK()
{
	int timeout = 200;
	char	reply_buf[256];

	if(atcmd_send("AT+NCONFIG=AUTOCONNECT,TRUE",timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("Auto connect failed\r\n");
		return -1;
	}
	printf("Auto connect OK\r\n");

	if(atcmd_check_value("AT+CFUN?", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The radio is not turn on\r\n");
		return -2;
	}
	printf("The radio is turned on\r\n");

	if(atcmd_check_value("AT+CSQ", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The module signal test failed,try again...\r\n");
		return -4;
	}
	strcpy(NBconf.CSQ,reply_buf);
	printf("The module signal test is normal.NBconf.CSQ:%s\r\n",NBconf.CSQ);


	if(atcmd_send("AT+CGATT=1", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The module attachment network test failed,try again...\r\n");
		return -3;
	}
	printf("The module attachment network test is normal\r\n");

	if(atcmd_check_value("AT+CGATT?", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The module attachment network test failed,try again...\r\n");
		return -3;
	}
	printf("The module attachment network test is normal\r\n");


	if(atcmd_check_value("AT+CEREG?", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The network registration status is abnormal,try again...\r\n");
		return -3;
	}
	printf("The network registration status is normal\r\n");

	if(atcmd_check_value("AT+CGPADDR", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("Obtaining an IP address is abnormal,try again...\r\n");
		return -3;
	}
	printf("Obtaining an IP address is normal\r\n");

	if(atcmd_send("AT+NCDP=221.229.214.202,5683\r\n",timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The module fails to connect to the cloud platform failed, try again...\r\n");
		return -4;
	}
	printf("The module connect to the cloud platform successfully.\r\n");

	if(atcmd_check_value("AT+NCDP?", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("The cloud platform is incorrectly configured, try again...\r\n");
		return -5;
	}
	printf("The cloud platform is correctly configured.\r\n");

	if(atcmd_check_value("AT+NMSTATUS?", timeout,reply_buf,sizeof(reply_buf))<0)
	{
		printf("Failed to register the telecom cloud platform,try again...\r\n");
		return -6;
	}
	printf("Successfully registered on the telecom cloud platform\r\n");


	return 0;
}
