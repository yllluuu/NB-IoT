/*
 * NB-IoT_test.h
 *
 *  Created on: 2024年7月3日
 *      Author: 杨璐
 */

#ifndef INC_AT_BC28_H_
#define INC_AT_BC28_H_

#include "at_cmd.h"

enum
{
	STAT_INIT,
	STAT_PRESEND,
	STAT_CONF,
	STAT_RDY,
};

typedef struct NBiot_conf_s
{
	int		status;
	char	manufacturers[64];
	char	model[64];
	char	imei[64];
	char	SIM[64];
	char	csq[64];
}NBiot_conf_t;

#define NCDP			"221.229.214.202,5683"

extern NBiot_conf_t		NBconf;

extern int nb_reset_ok(comport_t *comport);
extern int nb_hdw_ok(comport_t *comport);
extern int nb_conf_ok(comport_t *comport);
extern int bc28_Check_AT(comport_t *comport);
extern int bc28_Reset(comport_t *comport);
extern int bc28_Get_MANUF(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Get_Module(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_IMEI(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_SIMCD(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Set_AUTOCNT(comport_t *comport);
extern int bc28_Check_CFUN(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_CSQ(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Set_Attach_Net(comport_t *comport);
extern int bc28_Check_Attach_Net(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_Reg_Status(comport_t *comport, char *reply_buf,size_t size);
extern int bc28_Check_IP(comport_t *comport, char *reply_buf,size_t size);
extern int bc28_Set_IP_PORT(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_IP_PORT(comport_t *comport, char *reply_buf, size_t size);
extern int bc28_Check_IOT(comport_t *comport, char *reply_buf, size_t size);
#endif /* INC_AT_BC28_H_ */
