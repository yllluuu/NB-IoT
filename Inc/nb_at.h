/********************************************************************************
 *      Copyright:  (C) 2024 niuchunmin 
 *                  All rights reserved.
 *
 *       Filename:  nb_at.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(16/07/24)
 *         Author:  Niu Chunmin <niuchunmin@2430815397@qq.com>
 *      ChangeLog:  1, Release initial version on "16/07/24 14:08:09"
 *                 
 ********************************************************************************/

#ifndef _NB_AT_H_
#define _NB_AT_H_

#include "at_cmd.h"
#include "comport.h"
enum
{
	DISABLE,
	ENABLE,
};

/* ---------Basic AT Command------- */

/* AT Command : AT+NRB,AT*/
extern int nbiot_reset(comport_t *comport);

/*  Basic AT command  */
static inline int check_at_ready(comport_t *comport);
  
/*  AT command :ATE1 OR ATE0  */
int nbiot_set_echo(comport_t *comport, int enable);

/*  AT Command:AT+CGMI-----Module exist or not */
int nbiot_module_exist(comport_t *comport);

/*   AT Command:AT+CIMI-----SIM exist or not */
int nbiot_sim_exist(comport_t *comport);

/*  AT Command:AT+NCONFIG=AUTOCONNECT,TRUE */
int nbiot_autoConnect(comport_t *comport);

/*  AT Command: AT+CSQ */
int nbiot_csq(comport_t *comport);


//   open cfun=1 */
int nbiot_set_cfun(comport_t *comport);

//  AT+NBAND=5,8 */
int nbiot_set_nband(comport_t *comport);


//  AT+CGATT=1 */
int nbiot_set_cgatt(comport_t *comport);

//  AT+CGATT? */
int nbiot_seek_cgatt(comport_t *comport);

//   AT+NPING=221.229.214.202 */
int nbiot_set_server(comport_t *comport);

//  AT+NCDP=221.229.214.202,5683 
int nbiot_set_prot(comport_t *comport);


// AT+QREGSWT=1  
int nbiot_set_qregswt(comport_t *comport);

//  AT+NMSTATUS? 
int nbiot_seek_nmstatus(comport_t *comport);

int nb_connect(comport_t *comport);

/*  ----------------NB-IoT presend ---------------*/
int  nb_status_present(comport_t *comport);

/*  ---------------NB-IoT Congif------------------ */
int nb_status_config(comport_t *comport);

#endif
