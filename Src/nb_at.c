/*********************************************************************************
 *      Copyright:  (C) 2024 LingYun IoT System Studio
 *                  All rights reserved.
 *
 *       Filename:  nb_at.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "20/07/24 13:30:07"
 *                 
 ********************************************************************************/

#include "nb_at.h"


static inline int check_at_ready(comport_t *comport)
{
	int             times = 6;
	int             ready = 0;

	while( times-- )
	{
		if( 0 == send_atcmd_check_ok(comport, "AT", 500) )
		{
			ready = 1;
			break;
		}
	}

	return ready;
}

int nbiot_reset(comport_t *comport)
{
	int             rv;
	/*  
		rv = send_atcmd(comport, "AT+NRB", 5000, "OK", AT_ERRSTR, NULL, 0);
		if( rv < 0)
		{
		log_error("send AT command to reset NB-IoT failed, rv=%d\n", rv);
		return -1;
		}
		*/

	if( check_at_ready(comport) )
	{
		log_info("send AT to reset NB-IoT and AT command ready\n");
		return 0;
	}
	else
	{
		log_info("send AT to reset NB-IoT but AT command not ready\n");
		return -3;
	}

}

/*  AT command :ATE1 OR ATE0  */
int nbiot_set_echo(comport_t *comport, int enable)
{
	char           *at = enable? "ATE1" : "ATE0";

	return send_atcmd_check_ok(comport, at, 500);
}

/*  AT Command:AT+CGMI-----Module exist or not */
int nbiot_module_exist(comport_t *comport)
{
	int rv;
	if( (rv = send_atcmd(comport, "AT+CGMI", 5000, "OK", AT_ERRSTR, NULL, 0) )< 0)
	{
		log_error("send AT+AGMI command to  failed, rv=%d\n", rv);
		return -1;
	}
	return 0;

}

/*    AT Command:AT+CIMI-----SIM exist or not */
int nbiot_sim_exist(comport_t *comport)
{
	if( send_atcmd(comport, "AT+CIMI", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)
	{
		log_error("send AT+CIMI command to  failed\n");
		return -2;
	}
	return 0;

}

/* AT+CGMM */
int nbiot_get_gmm(comport_t *comport)
{

	if( send_atcmd(comport, "AT+CGMM", 5000, "OK", AT_ERRSTR,      NULL, 0) < 0)
	{   
		log_error("send AT+CGMM command to failed, \n");
		return -2; 
	}   
	return 0;
}
/*   AT Command:AT+NCONFIG=AUTOCONNECT,TRUE */
int nbiot_autoConnect(comport_t *comport)
{
	if( send_atcmd(comport, "AT+NCONFIG=AUTOCONNECT,TRUE", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)
	{
		log_error("send AT+NCONFIG=AUTOCONNECT,TRUE command to  failed\n");
		return -1;
	}
	return 0;
}

/*   AT Command: AT+CSQ */
int nbiot_csq(comport_t *comport)
{
	char         reply[128];

	if( send_atcmd_check_value(comport, "AT+CSQ", 5000, reply, 128)<0 )
	{
		log_error("senf at+csq failure.\n");
		return -1;
	}
	log_info("signal is:%s\n",reply);

	return 0;

}

/*    open cfun=1 */
int nbiot_set_cfun(comport_t *comport)
{

	if( send_atcmd(comport, "AT+CFUN=1", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)
	{
		log_error("send AT+CFUN command to  failed\n");
		return -1;
	}
	return 0;
}

/*   AT+NBAND=5,8 */
int nbiot_set_nband(comport_t *comport)
{
	if( send_atcmd(comport, "AT+CFUN=0", 5000, "OK", AT_ERRSTR,    NULL, 0) < 0)
	{
		log_error("send AT+CFUN command to failed\n");
		return -1;
	}
	if( send_atcmd(comport, "AT+NBAND=5,8", 5000, "OK", AT_ERRSTR,    NULL, 0) < 0)
	{
		log_error("send AT+NBAND=5,8 command to failed\n");
		return -2;
	}
	if( send_atcmd(comport, "AT+CFUN=1", 5000, "OK", AT_ERRSTR,    NULL, 0) < 0)
	{
		log_error("send AT+CFUN command to failed\n");
		return -3;
	}
	return 0;


}

/*   AT+CGATT=1 */
int nbiot_set_cgatt(comport_t *comport)
{

	if( send_atcmd(comport, "AT+CGATT=1", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)                 
	{         
		log_error("send AT+CFUN command to  failed\n");
		return -1;
	}
	return 0;
}

/*   AT+CGATT? */
int nbiot_seek_cgatt(comport_t *comport)
{

	if( send_atcmd(comport, "AT+CGATT?", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)                 
	{         
		log_error("send AT+CFUN command to  failed\n");
		return -1;
	}
	return 0;
}

//   AT+NPING=221.229.214.202 */
int nbiot_set_server(comport_t *comport)
{
	if( send_atcmd(comport, "AT+NPING=221.229.214.202", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)                  
	{  
		log_error("send AT+NPING command to  failed\n");
		return -1;
	}
	return 0;

}

/*   AT+NCDP=221.229.214.202,5683 */
int nbiot_set_prot(comport_t *comport){
	if( send_atcmd(comport, "AT+NCDP=221.229.214.202,5683", 5000, "OK",    AT_ERRSTR, NULL, 0) < 0)                  
	{  
		log_error("send AT+NCDP command to  failed\n");
		return -1;
	}
	return 0;
}

/*   AT+NMSTATUS? */
int nbiot_seek_nmstatus(comport_t *comport)
{
	if( send_atcmd(comport, "AT+NMSTATUS?", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)                  
	{  
		log_error("send AT+CNMSTATUS? command to  failed\n");
		return -1;
	}
	return 0;

}

/* 手动联网 */
int nb_connect(comport_t *comport)
{
	if( send_atcmd(comport, "AT+NCONFIG=AUTOCONNECT,FALSE", 5000, "OK", AT_ERRSTR, NULL, 0) < 0)                       
	{
		log_error("send CONNECT command to   failed\n");
		return -1;
	}
	return 0;


}
#if 0
int  nb_status_present(comport_t *comport)
{
	int rv=0;

	rv = send_atcmd(comport, "AT+CGMI", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+AGMI command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGMM", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CGMM command to  failed, rv=%d\n", rv);
		return -2;
	}


	rv = send_atcmd(comport, "AT+CIMI", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CIMI command to  failed, rv=%d\n", rv);
		return -3;
	}

	return 0;
}
#endif

int nb_status_present(comport_t *comport)
{
	if( nbiot_module_exist(comport)<0 )
	{
		
		printf ("nb module not exist\n");
		return -1;
	}
	if( nbiot_sim_exist(comport) <0 )
	{

		printf ("nb sim not exist!\n");
		return -2;
	}
	if( nbiot_get_gmm(comport) <0 )
	{

		printf ("nb gmm not exist!\n");
		return -3;
	}
	return 0;
}

#if 0
int nb_status_config(comport_t *comport)
{
	int rv;

	rv = send_atcmd(comport, "AT+NCONFIG=AUTOCONNECT,TRUE", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+NCONFIG=AUTOCONNECT,TRUE command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CFUN=1", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)
	{
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGATT=1", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGATT?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CEREG?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+CGPADDR", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                 
	{         
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NCDP=221.229.214.202,5683", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NCDP?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	rv = send_atcmd(comport, "AT+NMSTATUS?", 5000, "OK", AT_ERRSTR, NULL, 0);
	if( rv < 0)                  
	{  
		log_error("send AT+CFUN command to  failed, rv=%d\n", rv);
		return -1;
	}

	return 0;

}
#endif

int nb_status_config(comport_t *comport)
{
	if( nbiot_autoConnect(comport)<0 )
	{
		printf ("nbiot auto connect failure\n");
		return -1;
	}

	if(nbiot_set_cfun(comport)<0 )
	{

		printf ("nbiot set cfun failed\n");
		return -2;
	}

	if( nbiot_set_cgatt(comport)<0 )
	{

		printf ("nbiot set cgatt failed\n");
		return -3;
	}

	if( nbiot_csq(comport)<0 )
	{

		printf ("nbiot csq:99,99\n");
		return -8;
	}
	if( nbiot_set_server(comport) <0 )
	{

		printf ("nbiot set server failed!\n");
		return -4;
	}


	if( nbiot_set_prot(comport) <0 )
	{

		printf ("nbiot set port failed!\n");
		return -5;
	}

	if( nbiot_seek_nmstatus(comport)<0 )
	{

		printf ("nbiot seek nbstatus failed\n");
		return -6;
	}

	return 0;
}
